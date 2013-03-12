#!/bin/bash

SUM_OUT="modules.h"
ALL_HEADERS="modules_headers.h"

function processModuleLine
{
#;MODULE NAME TYPE
    CurModuleName="${2}"
    CurModuleObjectName="${2}_Mod"
    ModuleFileName="${2}.h"
    echo -n "const char *${CurModuleName}_Src = " > $ModuleFileName
    echo "#include \"${2}.h\"" >> $ALL_HEADERS
    
    
    if [ "$3" == "GENERATOR" ]
    then
        echo "Generator *${2}_Mod = new Generator(\"$2\");" >> $SUM_OUT
    elif [ "$3" == "OUTPUT" ]
    then
        echo "Output *"${2}"_Mod = new Output(\"$2\");" >> $SUM_OUT
    elif [ "$3" == "FUNCTION" ]
    then
	echo "Function *"${2}"_Mod = new Function(\"$2\");" >> $SUM_OUT
    elif [ "$3" == "MODIFIER" ]
    then
	echo "Modifier *"${2}"_Mod = new Filter(\"$2\");" >> $SUM_OUT
    fi
    
    echo "${2}_Mod->setModuleSource(${2}_Src);" >> $SUM_OUT
}

function processEndModuleLine
{
	echo -n ";" >> $ModuleFileName
	echo "availableModules.insert(std::make_pair(\"$CurModuleName\", $CurModuleObjectName));" >> $SUM_OUT
}

function processProtoLine
{
    echo -n "${CurModuleObjectName}->setProto(\"" >> $SUM_OUT
    for i in $@
    do
        if [ "$i" != ";PROTO" ]
        then
	    echo -n "$i " >> $SUM_OUT
        fi
    done
    echo "\n\");" >> $SUM_OUT
}

function processArgLine
{
#;ARG ID TYPE NAME DEF
    if [ "$3" == "float" ]
    then
        echo "${CurModuleObjectName}->setAttribute($2, Attribute(\"$4\", ${5}f, ${6}f, ${7}f));" >> $SUM_OUT
    elif [ "$3" == "int" ]
    then
        echo "${CurModuleObjectName}->setAttribute($2, Attribute(\"$4\", ${5}, ${6}, ${7}));" >> $SUM_OUT
    fi

}

function processDepLine
{
	echo "${CurModuleObjectName}->addDependency(\"${2}\");" >> $SUM_OUT;
}

function processOutType
{
	echo "${CurModuleObjectName}->setOutputType(BaseModule::ContactInfo::${2});" >> $SUM_OUT
}

function processInputLine
{
#;INPUT NAME TYPE
	echo "${CurModuleObjectName}->addInput(BaseModule::ContactInfo(\"${2}\", BaseModule::ContactInfo::${3}));" >> $SUM_OUT
}

function processModule
{
	if [ "$1" == ";ARG" ]
	then
		processArgLine ${*}
	elif [ "$1" == ";DEP" ]
	then
		processDepLine ${*}
	elif [ "$1" == ";OUTTYPE" ]
	then
		processOutType ${*}
	elif [ "$1" == ";INPUT" ]
	then
		processInputLine ${*}
	elif [ "$1" == ";PROTO" ]
	then
		processProtoLine ${*}
	fi
	
}

>$SUM_OUT
>$ALL_HEADERS

for a in *.cl
do

    moduleStarted=0

    while IFS= read -r line
    do

	for part in $line
	do
	    if [ "$part" == ";MODULE" ]
	    then
		moduleStarted=1
		processModuleLine $line
	    elif [ "$part" == ";ENDMODULE" ]
	    then
		moduleStarted=0
		processEndModuleLine $line
		
	    fi
    	    
    	    break
	done

        if [ "$moduleStarted" == "1" ]
        then
		if [ "${line:0:1}" != ";" ]
		then
			echo "\"$line\n\"" >> $ModuleFileName
		else
			processModule $line
		fi
    	fi
	
    done < $a
    
done
