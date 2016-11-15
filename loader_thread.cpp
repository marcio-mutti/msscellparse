#include "loader_thread.hpp"

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
    regexers.insert ( {"loggername",regex{"(?:MSCi +)(\\w+)(?: +)(\\d{4}-\\d{2}-\\d{2} +\\d{2}\\:\\d{2}\\:\\d{2})"}} );
    regexers.insert ( {"bscname",regex{"(?:BSC NAME )(?:\\.*)(?:\\(NAME\\)\\. \\:)(\\w+)"}} );
    regexers.insert ( {"cell_2g",regex{"(?:^ +)(\\w+)(?: +\\d+ +)(\\d+)(?: +)(\\d+)(?: +)(\\d+)(?: +)(\\d+)(?: +)(\\w+)"}} );
    regexers.insert ( {"rncid",regex{"(?:RNC IDENTIFICATION\\.+ RNCID \\.+ \\: )(\\d+)"}} );
    regexers.insert ( {"rncname",regex{"(?:RNC NAME\\.+ RNCNAME \\. \\: )(\\w+)"}} );
    regexers.insert ( {"rnc_sac",regex{"(\\d+)(?: +724 +)(\\d+)"}} );
    regexers.insert ( {"command_end",regex{"COMMAND +EXECUTED"}} );
    regexers.insert ( {"cell_3g",regex{"(\\w+)(?: +\\d+ +)(\\d+)(?: +)(\\w+)"}} );
    regexers.insert ( {"enodeb_id",regex{"(?:ENB IDENTIFICATION\\.+\\(ENBID\\)\\.+ \\: )(\\d+)"}} );
    regexers.insert ( {"inodeb_ip",regex{"(?:ENB IP ADDRESS\\.+\\(ENBIP\\)\\.+ \\: )(\\d+\\.\\d+\\.\\d+\\.\\d+)"}} );
    regexers.insert ( {"tac",regex{"(?:TRACKING AREA CODE\\.+\\(TAC#1 \\)\\.+ \\: )(\\d+)"}} );
    triggers.insert ( {"cell_3g",regex{"SERVICE AREAS IN MSS CONCEPT \\:"}} );
}
logparser::parser::~parser()	{}
void logparser::parser::slot_new_file ( const string& filename, const logtype& type_of_logger ) throw ( const domain_error& ) {
    switch ( type_of_logger ) {
    case logtype::mobswitch:
        lista_de_mobswitches.push_back ( filename );
        break;
    default:
        string message ( "Tipo de elemento de rede incorreto. Informado: " );
        message += to_string ( type_of_logger ) + '\n';
        throw ( domain_error ( message ) );
    }
}
void logparser::parser::slot_run() throw ( const runtime_error& ) {

}





