#!/bin/bash

SUM_OUT="modules.h"
ALL_HEADERS="modules_headers.h"

function processNameLine 
{
#;MODULE NAME TYPE A 6 I 0 O 1 C 0
    CurModuleName="${2}_Mod"
    if [ "$3" == "BASE" ]
    then
        echo "NoiseModule *"$name"_Mod = new NoiseModule($5, $7, $9, ${11}, \"$2\", "$2"Src, this);" >> $SUM_OUT
    elif [ "$3" == "OUTPUT" ]
    then
        echo "NoiseOutput *"$name"_Mod = new NoiseOutput($5, $7, $9, \"$2\", "$2"Src, this);" >> $SUM_OUT
    fi

    echo "availableModules.insert(std::make_pair(\"${2}\", ${name}_Mod));" >> $SUM_OUT
}

function processArgLine
{
#;ARG ID TYPE NAME DEF
    if [ "$3" == "float" ]
    then
        echo "${CurModuleName}->setAttribute($2, NoiseModuleAttribute(\"$4\", ${5}f));" >> $SUM_OUT
    elif [ "$3" == "int" ]
    then
        echo "${CurModuleName}->setAttribute($2, NoiseModuleAttribute(\"$4\", ${5}));" >> $SUM_OUT
    fi

}

#function processOutputLine
#{
#}

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
    	    else
		echo "\"$line\"">>$outname
		break
    	    fi
	done
    done < $a
echo ";">>$outname
echo $MODULE_NAME
done