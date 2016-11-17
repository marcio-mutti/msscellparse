#include "loader_thread.hpp"

#include <functional>

#define N_CARACTERES_ESPERADO_LINHA 150

using namespace std;

string logparser::readline ( ifstream& filehandle ) {
    string resultado;
    char byte ( 0 );
    resultado.reserve ( N_CARACTERES_ESPERADO_LINHA );
    while ( filehandle.good() ) {
        filehandle.read ( &byte,1 );
        if ( byte == '\n' ) break;
        else {
            if ( byte == '\r' ) continue;
            resultado.push_back ( byte );
        }
    }
    return resultado;
}

logparser::parser::parser()		{
    regexers.insert ( {"switchname",regex{"(?:MSCi +)(\\w+)(?: +)(\\d{4}-\\d{2}-\\d{2} +\\d{2}\\:\\d{2}\\:\\d{2})"}} );
    regexers.insert ( {"cell_2g",regex{"(?:^ +)(\\w+)(?: +\\d+ +)(\\d+)(?: +)(\\d+)(?: +)(\\d+)(?: +)(\\d+)(?: +)(\\w+)"}} );
    regexers.insert ( {"rncname",regex{"(?:RNC NAME\\.+ RNCNAME \\. \\: )(\\w+)"}} );
    regexers.insert ( {"rnc_sac",regex{"(\\d+)(?: +724 +)(\\d+)"}} );
    regexers.insert ( {"command_end",regex{"COMMAND +EXECUTED"}} );
    regexers.insert ( {"cell_3g",regex{"(\\w+)(?: +\\d+ +)(\\d+)(?: +)(\\w+)"}} );
    regexers.insert ( {"enodeb_id",regex{"(?:ENB IDENTIFICATION\\.+\\(ENBID\\)\\.+ \\: )(\\d+)"}} );
    regexers.insert ( {"inodeb_ip",regex{"(?:ENB IP ADDRESS\\.+\\(ENBIP\\)\\.+ \\: )(\\d+\\.\\d+\\.\\d+\\.\\d+)"}} );
    regexers.insert ( {"tac",regex{"(?:TRACKING AREA CODE\\.+\\(TAC#1 \\)\\.+ \\: )(\\d+)"}} );
    triggers.insert ( {"cell_3g",regex{"SERVICE AREAS IN MSS CONCEPT \\:"}} );
    triggers.insert ( {"cell_2g",regex{"BTSs UNDER BSC \\:"}} );
    triggers.insert ( {"bscname",regex{"(?:BSC NAME )(?:\\.*)(?:\\(NAME\\)\\. \\:)(\\w+)"}} );
    triggers.insert ( {"rncid",regex{"(?:RNC IDENTIFICATION\\.+ RNCID \\.+ \\: )(\\d+)"}} );
    triggers.insert ( {"rnc_load_lac",regex{"(?:LA +NAME \\:LAC)(\\d+)(?: +LAC +\\: *)(\\d+)"}} );
}
logparser::parser::~parser()	{}
void logparser::parser::slot_new_file ( const string& filename, const logtype& type_of_logger ) throw ( const domain_error& ) {
    switch ( type_of_logger ) {
    case logtype::mobswitch:
        lista_de_mobswitches.push_back( filename );
        break;
    case logtype::mme:
        lista_de_mmes.push_back( filename );
        break;
    default:
        string message ( "Tipo de elemento de rede incorreto. Informado: " );
        message += to_string ( type_of_logger ) + '\n';
        throw ( domain_error ( message ) );
    }
    signal_n_of_mobswitches_.emit(to_string(lista_de_mobswitches.size()));
}
sigc::signal<void, const string&> logparser::parser::signal_n_of_mobswitches()  { return signal_n_of_mobswitches_; }
sigc::signal<void> logparser::parser::signal_work_finish()                      { return signal_work_finish_; }

void logparser::parser::slot_run() throw ( const runtime_error& ) {
    for (vector<string>::iterator siter=lista_de_mobswitches.begin(); siter!=lista_de_mobswitches.end();++siter) {
        shared_ptr<::mobswitch> work_switch= make_shared<::mobswitch>();
        working_switches.push_back(work_switch);
        threads_de_execucao.insert({*siter,make_shared<boost::thread>([&](){parse_switch(work_switch,cref(*siter));})});
        //parse_switch(work_switch,cref(*siter));
    }
    for (vector<string>::iterator miter=lista_de_mmes.begin(); miter!=lista_de_mmes.end();++miter) {
        shared_ptr<::mme> work_mme= make_shared<::mme>();
        working_mmes.push_back(work_mme);
        threads_de_execucao.insert({*miter,make_shared<boost::thread>([&](){parse_mme(work_mme,cref(*miter));})});
        //parse_switch(work_switch,switch_filename);
    }
    for (map<string,shared_ptr<boost::thread>>::iterator titer=threads_de_execucao.begin();titer!=threads_de_execucao.end();++titer) {
        titer->second->join();
    }
    lista_de_mobswitches.clear();
    lista_de_mmes.clear();
    signal_work_finish_.emit();
    boost::this_thread::sleep_for(boost::chrono::milliseconds(400));
}

void logparser::parser::parse_switch(std::shared_ptr< ::mobswitch> work_switch, const string& filename) {
    //DEBUG
    cout << filename << " sendo tratado na thread " << boost::this_thread::get_id() << endl;
    //ENDDEBUG
    ifstream workfile;
    workfile.open(filename,ios_base::in);
    bool switch_identified(false);
    bool t_bsc(false), t_rnc(false), t_cell2g(false), t_cell3g(false);
    vector<bsc>::iterator it_work_bsc;
    vector<rnc>::iterator it_work_rnc;
    int work_lac_sac(-1);
    smatch matches;
    if (workfile.fail()) {
        cerr << "Não foi possível abrir o arquivo " << filename << "." << endl << "Abortando Arquivo." << endl;
        return;
    }
    while (workfile.good()) {
        string linha(readline(workfile));
        if (linha.size() < 4) continue;
        //My first step shall be identify the switch. While it's not possible, theres no sense in continuing
        if (!switch_identified) {
            if (regex_search(linha,matches,regexers.at("switchname"))) {
                switch_identified=true;
                work_switch->set_name(matches[1]);
            }
            continue;
        }
        //Switch is identified, now check if I should stop any triggers. There should be triggers for bsc, cell_2g, rnc and cell_3g
        if (regex_search(linha,matches,regexers.at("command_end"))) {
            t_bsc=t_rnc=t_cell2g=t_cell3g=false;
            continue;
        }
        //Now we should check if we find any of the triggers
        //NOTE 1 The trigger for bsc is already its researcher. Lets start with it.
        if (regex_search(linha,matches,triggers.at("bscname"))) {
            t_bsc=true;
            bsc n_bsc;
            n_bsc.set_name(matches[1]);
            it_work_bsc=work_switch->add_bsc(n_bsc);
            continue;
        }
        if (regex_search(linha,matches,triggers.at("cell_2g"))) {
            if (!t_bsc) throw (runtime_error("Começaram celulas 2g sem descobrir uma bsc. Travando em 3, 2, 1..."));
            t_cell3g=t_rnc=false;
            t_cell2g=true;
            continue;
        }
        if(regex_search(linha,matches,triggers.at("rnc_load_lac"))) {
            t_cell2g=t_bsc=t_rnc=false;
            work_lac_sac=stoi(matches[1]);
            continue;
        }
        if(regex_search(linha,matches,triggers.at("cell_3g"))) {
            if (work_lac_sac < 0) throw(runtime_error("Começaram celulas 3g sem que fosse possível descobrir o SAC. Travando em 3, 2, 1..."));
            t_cell2g=t_bsc=t_rnc=false;
            t_cell3g=true;
            continue;
        }
        if (regex_search(linha,matches,triggers.at("rncid"))) {
            t_bsc=t_cell2g=t_cell3g=false;
            //it_work_bsc=nullptr; TODO See if something like this is really necessary
            t_rnc=true;
            rnc n_rnc;
            n_rnc.set_id(matches[1]);
            it_work_rnc=work_switch->add_rnc(n_rnc);
            continue;
        }
        //From this point on we grab the available information
        if (t_cell2g) {
            if (regex_search(linha,matches,regexers.at("cell_2g"))) {
                celula n_celula;
                n_celula.set_name(matches[1]);
                n_celula.set_cgi(matches[3],matches[4],matches[2],matches[5]);
                n_celula.set_status(matches[6]);
                it_work_bsc->add_celula(n_celula);
                continue;
            }
        }
        if (t_rnc) {
            if (regex_search(linha,matches,regexers.at("rncname"))) {
                it_work_rnc->set_name(matches[1]);
                continue;
            }
            if (regex_search(linha,matches,regexers.at("rnc_sac"))) {
                it_work_rnc->add_lac_sac(matches[1]);
                it_work_rnc->set_mnc(matches[2]);
                continue;
            }
        }
        if (t_cell3g) {
            if (regex_search(linha,matches,regexers.at("cell_3g"))) {
                celula n_celula;
                n_celula.set_name(matches[1]);
                n_celula.set_mcc(724);
                n_celula.set_mnc(it_work_rnc->get_mnc());
                n_celula.set_lac_sac(work_lac_sac);
                n_celula.set_cid(stoi(matches[2]));
                n_celula.set_status(matches[3]);
                it_work_rnc->add_celula(n_celula);
                continue;
            }
        }
    }
    workfile.close();
}
void logparser::parser::parse_mme(std::shared_ptr< ::mme>, const string& filename) {
}

