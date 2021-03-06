#########################################################
# CGB, 20100716, created
#########################################################

python.get <- function(var.name, string.code=FALSE, ...){

    python.command <- paste0( "_r_return = object2json( ",
                             ifelse(string.code, var.name, deparse(substitute(var.name))),
                             " )" )
    python.exec( python.command, get.exception = TRUE, string.code=TRUE )

    ret <- .C( "py_get_var", "_r_return", not.found.var = integer(1), resultado = character(1), PACKAGE = "rPython" )

    if( ret$not.found.var )
        stop( "Variable not found" )
        
    return(fromJSON(gsub('\\<nan?\\>','null',ret$resultado,ignore.case = T), ...))
}

