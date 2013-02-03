#!/bin/bash

SUM_OUT="modules.h"
ALL_HEADERS="modules_headers.h"

function processNameLine 
{
#;MODULE NAME TYPE A 6 I 0 O 1 C 0
    CurModuleName="${2}_Mod"
    if [ "$3" == "BASE" ]
    then
        echo "Module *"$name"_Mod = new Module($5, $7, $9, ${11}, \"$2\", "$2"Src);" >> $SUM_OUT
    elif [ "$3" == "OUTPUT" ]
    then
        echo "Output *"$name"_Mod = new Output($5, $7, $9, \"$2\", "$2"Src);" >> $SUM_OUT
    elif [ "$3" == "FUNCTION" ]
    then
	echo "Function *"$name"_Mod = new Function(\"$2\", "$2"Src);" >> $SUM_OUT
    fi

    echo "availableModules.insert(std::make_pair(\"${2}\", ${name}_Mod));" >> $SUM_OUT
}

function processProtoLine
{
    echo -n "${CurModuleName}->setProto(\"" >> $SUM_OUT
    for i in $@
    do
        if [ "$i" != ";PROTO" ]
        then
	    echo -n "$i " >> $SUM_OUT
        fi
    done
    echo "\");" >> $SUM_OUT
}

function processArgLine
{
#;ARG ID TYPE NAME DEF
    if [ "$3" == "float" ]
    then
        echo "${CurModuleName}->setAttribute($2, ModuleAttribute(\"$4\", ${5}f, ${6}f, ${7}f));" >> $SUM_OUT
    elif [ "$3" == "int" ]
    then
        echo "${CurModuleName}->setAttribute($2, ModuleAttribute(\"$4\", ${5}, ${6}, ${7}));" >> $SUM_OUT
    fi

}

function processDepLine
{
    echo "${CurModuleName}->addDependency(\"${2}\");" >> $SUM_OUT;
}

>$SUM_OUT
>$ALL_HEADERS

for a in *.cl
do

    name=`echo $a | cut -f1 -d'.'`
    outname=$name."h"

    echo "#include \"$name.h\"" >> $ALL_HEADERS
    echo "const char "$name"Src[] = " >$outname
    while IFS= read -r line
    do

	for part in $line
	do
    	    if [ "$part" == ";MODULE" ]
	    then
		processNameLine $line
		break
    	    elif [ "$part" == ";ARG" ]
    	    then
    		processArgLine $line
		break
    	    elif [ "$part" == ";PROTO" ]
	    then
		processProtoLine $line
		break
    	    elif [ "$part" == ";DEP" ]
	    then
		processDepLine $line
		break
    	    else
		echo "\"$line\\n\"">>$outname
		break
    	    fi
	done
    done < $a
echo ";">>$outname
echo $MODULE_NAME
done