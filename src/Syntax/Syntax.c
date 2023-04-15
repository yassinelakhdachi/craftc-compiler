#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "Syntax.h"

void get_token()
{
	char tmp[20];
	char c = getc(tokens_file);
	int i = 0;
	while (c != '\n')
	{
		tmp[i] = c;
		c = getc(tokens_file);
		i++;
	}
	tmp[i] = '\0';
	currentToken = (char *)malloc(i + 1);
	strcpy(currentToken, tmp);
	//printf("%s \n", currentToken);
}
void error(char *message, char **stop_token)
{
	bool no_stop = TRUE;
	fprintf(error_file, "syntax error: %s\n", message);
	fflush(error_file);
	//printf("inside error");
	do
	{
		for (int i = 0; stop_token[i] != "\0"; i++)
		{
			if (strcmp(currentToken, stop_token[i]) == 0)
			{
				no_stop = FALSE;
				//printf("leaving error");
				return;
			}
		}
		if (no_stop == TRUE)
			get_token();
	} while (no_stop);
	//printf("leaving error");
}
void program()
{
	get_token();
	if (strcmp(currentToken, "BIBLIO_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "DP_TOKEN") != 0)
			error("attendant ':' après Biblio", (char *[]){"PV_TOKEN", "\0"});
		libraries();
	}
	else
		error("block biblio manquant", (char *[]){"CONST_TOKEN", "VAR_TOKEN", "FUNC_TOKEN", "PRINC_TOKEN", "\0"});
	if (strcmp(currentToken, "CONST_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "DP_TOKEN") != 0)
			error("attendant ':' après Constantes", (char *[]){"PV_TOKEN", "\0"});
		constants();
	}else
		error("block constantes manquant", (char *[]){"VAR_TOKEN", "FUNC_TOKEN", "PRINC_TOKEN", "\0"});
	if (strcmp(currentToken, "VAR_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "DP_TOKEN") != 0)
			error("attendant ':' après Variables", (char *[]){"PV_TOKEN", "\0"});
		variables();
	}else
		error("block variables manquant", (char *[]){"FUNC_TOKEN", "PRINC_TOKEN", "\0"});
	if (strcmp(currentToken, "FUNC_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "DP_TOKEN") != 0)
			error("attendant ':' après Fonctions", (char *[]){"ID_TOKEN", "PRINC_TOKEN", "\0"});
		else
			get_token();
		do
		{
			functions();
		} while (strcmp(currentToken, "ID_TOKEN") == 0);
	}else
		error("block fonctions manquant", (char *[]){"PRINC_TOKEN", "\0"});
	if (strcmp(currentToken, "PRINC_TOKEN") == 0)
	{
		Main();
		return;
	}
	else
		error("bloc Principale manquant", (char *[]){"\0"});
}
void libraries()
{
	get_token();
	while (strcmp(currentToken, "BIB_TOKEN") == 0 || strcmp(currentToken, "ENTETE_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "DP_TOKEN") != 0)
		{
			error("attendant ':' après bib/entete", (char *[]){"PV_TOKEN", "\0"});
			get_token();
			continue;
		}
		do
		{
			get_token();
			if (strcmp(currentToken, "ID_TOKEN") != 0)
			{
				error("nom de biblio ou entete manquant", (char *[]){"PV_TOKEN", "\0"});
				break;
			}
			get_token();
		} while (strcmp(currentToken, "VIR_TOKEN") == 0);
		if (strcmp(currentToken, "PV_TOKEN") != 0)
			error("';' manquant dans biblio", (char *[]){"BIB_TOKEN", "ENTETE_TOKEN", "CONST_TOKEN", "VAR_TOKEN", "FUNC_TOKEN", "PRINC_TOKEN", "\0"});
		else
			get_token();
	}
}
void constants()
{
	get_token();
	while (strcmp(currentToken, "ID_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "AFF_TOKEN") != 0)
		{
			error("identificateur d'affectation manquant dans Constantes", (char *[]){"ID_TOKEN", "VAR_TOKEN", "FUNC_TOKEN", "PRINC_TOKEN", "\0"});
			continue;
		}
		get_token();
		is_value();
		//is_value reads next token too
		if (strcmp(currentToken, "PV_TOKEN") != 0)
		{
			error("attendant ';' après affectation dans Constantes", (char *[]){"ID_TOKEN", "VAR_TOKEN", "FUNC_TOKEN", "PRINC_TOKEN", "\0"});
			continue;
		}
		get_token();
	}
}
void variables()
{
	get_token();
	while (strcmp(currentToken, "ID_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "AFF_TOKEN") == 0)
		{
			get_token();
			is_value();
		}
		if (strcmp(currentToken, "PV_TOKEN") == 0 || strcmp(currentToken, "VIR_TOKEN") == 0)
		{
			get_token();
			continue;
		}
		else
			error("attendant ';' après declaration dans Variables", (char *[]){"ID_TOKEN", "FUNC_TOKEN", "PRINC_TOKEN", "\0"});
	}
}
//-------FUNC--------------------
void functions()
{
	if (strcmp(currentToken, "ID_TOKEN") != 0)
		error("nom de la fonction manquant dans Fonctions", (char *[]){"PO_TOKEN", "\0"});
	else
		get_token();
	if (strcmp(currentToken, "PO_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "ID_TOKEN") == 0)
		{
			get_token();
			while (strcmp(currentToken, "VIR_TOKEN") == 0)
			{
				get_token();
				if (strcmp(currentToken, "ID_TOKEN") != 0)
				{
					error("Argument manquant dans la déclaration de la fonction", (char *[]){"PF_TOKEN", "CBO_TOKEN", "\0"});
					break;
				}
				else
					get_token();
			}
		}
		if (strcmp(currentToken, "PF_TOKEN") != 0)
			error("attendant ')' après les arguments dans la déclaration de la fonction dans Fonctions", (char *[]){"CBO_TOKEN", "\0"});
		else
			get_token();
	}
	else
		error("attendant '(' avant le nom de la fonction dans Fonctions", (char *[]){"CBO_TOKEN", "\0"});

	if ((strcmp(currentToken, "CBO_TOKEN") == 0))
	{
		get_token();
		if (strcmp(currentToken, "CONST_TOKEN") == 0)
		{
			get_token();
			if (strcmp(currentToken, "DP_TOKEN") != 0)
				error("attendant ':' après Constantes", (char *[]){"PV_TOKEN", "\0"});
			constants();
		}
		if (strcmp(currentToken, "VAR_TOKEN") == 0)
		{
			get_token();
			if (strcmp(currentToken, "DP_TOKEN") != 0)
				error("attendant ':' après Variables", (char *[]){"PV_TOKEN", "\0"});
			variables();
		}
		Insts();
	}
	else
		error("attendant '{' dans la déclaration de la fonction", (char *[]){"CBF_TOKEN", "PRINC_TOKEN", "\0"});
	if ((strcmp(currentToken, "CBF_TOKEN") != 0))
		error("attendant '}' dans la fin de la déclaration de la fonction", (char *[]){"PRINC_TOKEN", "\0"});
	else
		get_token();
}
//------------INSTS-----------
void Insts()
{
	while (Inst())
		;
}
//-----------INST------------
bool Inst()
{
	if (strcmp(currentToken, "ID_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "AFF_TOKEN") == 0)
			expr();
		else if (strcmp(currentToken, "PO_TOKEN") == 0)
			CallFunction();
		else if (strcmp(currentToken, "BO_TOKEN") == 0)
			list();
		else
			error("instruction n'est pas claire", (char *[]){"PV_TOKEN", "RETURN_TOKEN", "SI_TOKEN", "POUR_TOKEN", "FAIRE_TOKEN", "TANTQUE_TOKEN", "\0"});
		if (strcmp(currentToken, "PV_TOKEN") != 0)
			error("attendant ';' à la fin de l'instruction", (char *[]){"RETURN_TOKEN", "SI_TOKEN", "POUR_TOKEN", "FAIRE_TOKEN", "TANTQUE_TOKEN", "\0"});
		else
			get_token();
	}
	else if (strcmp(currentToken, "RETURN_TOKEN") == 0)
	{
		expr();
		if (strcmp(currentToken, "PV_TOKEN") != 0)
			error("attendant ';' à la fin de l'instruction", (char *[]){"RETURN_TOKEN", "SI_TOKEN", "POUR_TOKEN", "FAIRE_TOKEN", "TANTQUE_TOKEN", "ID_TOKEN", "\0"});
		else
			get_token();
	}
	else if (strcmp(currentToken, "SI_TOKEN") == 0)
	{
		decision();
	}
	else if (strcmp(currentToken, "POUR_TOKEN") == 0 || strcmp(currentToken, "FAIRE_TOKEN") == 0 || strcmp(currentToken, "TANTQUE_TOKEN") == 0)
	{
		loop();
	}
	else
		return FALSE;
	return TRUE;
}
//------------------LIST------
void list()
{
	get_token();
	if (strcmp(currentToken, "NUM_TOKEN") != 0 && strcmp(currentToken, "ID_TOKEN") != 0)
		error("attendant index après '[' dans l'instruction", (char *[]){"BF_TOKEN", "AFF_TOKEN", "PV_TOKEN", "\0"});
	else
		get_token();
	if (strcmp(currentToken, "BF_TOKEN") != 0)
		error("attendant ']' après l'indice dans l'instruction", (char *[]){"AFF_TOKEN", "PV_TOKEN", "\0"});
	else
		get_token();
	if (strcmp(currentToken, "AFF_TOKEN") == 0)
		expr();
	else
		error("affectation manquante dans l'instruction", (char *[]){"PV_TOKEN", "\0"});
}
//-----------CALLFUNCTION-------- start by PO_token already read
void CallFunction()
{
	if (strcmp(currentToken, "PO_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "ID_TOKEN") == 0)
		{
			get_token();
			while ((strcmp(currentToken, "VIR_TOKEN") == 0))
			{
				get_token();
				if (strcmp(currentToken, "ID_TOKEN") != 0)
					error("argument missing when calling function", (char *[]){"PF_TOKEN", "PV_TOKEN", "\0"});
				get_token();
			}
		}
	}
	else
		error("expecting '(' after function's name", (char *[]){"PF_TOKEN", "PV_TOKEN", "\0"});
	if (strcmp(currentToken, "PF_TOKEN") != 0)
		error("expecting ')' after function's arguments", (char *[]){"PV_TOKEN", "\0"});
	else
		get_token();
}
//-------MAIN--------------------
void Main()
{
	get_token();
	if (strcmp(currentToken, "PO_TOKEN") != 0)
		error("attendant '(' après Principale", (char *[]){"PF_TOKEN", "DP_TOKEN", "ID_TOKEN", "RETURN_TOKEN", "SI_TOKEN", "POUR_TOKEN", "FAIRE_TOKEN", "TANTQUE_TOKEN", "\0"});
	else
		get_token();
	while (strcmp(currentToken, "ID_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "VIR_TOKEN") == 0)
		{
			get_token();
			continue;
		}
		else
			break;
	}
	if (strcmp(currentToken, "PF_TOKEN") != 0)
		error("attendant ')' après les arguments de principale ", (char *[]){"DP_TOKEN", "ID_TOKEN", "RETURN_TOKEN", "SI_TOKEN", "POUR_TOKEN", "FAIRE_TOKEN", "TANTQUE_TOKEN", "\0"});
	else
		get_token();
	if (strcmp(currentToken, "DP_TOKEN") != 0)
		error("attendant':' après Principale(args)", (char *[]){"ID_TOKEN", "RETURN_TOKEN", "SI_TOKEN", "POUR_TOKEN", "FAIRE_TOKEN", "TANTQUE_TOKEN", "\0"});
	else
		get_token();
	Insts();
}
//Condition:(exp Comp_Op exp) {logic_op (exp com_op exp)}
void condition()
{
	do
	{
		get_token();
		if (strcmp(currentToken, "PO_TOKEN") == 0)
		{
			expr();
			int i;
			for (i = 0; i < 6; i++)
			{
				if (strcmp(currentToken, Comp_Op[i]) == 0)
					break;
			}
			if (i == 6)
				error("expecting comparaison inside condition", (char *[]){"PF_TOKEN", "CBO_TOKEN", "PV_TOKEN", "\0"});
			else
				expr();
			if (strcmp(currentToken, "PF_TOKEN") != 0)
				error("expecting ')' at the end of each condition", (char *[]){"CBO_TOKEN", "PV_TOKEN", "\0"});
			else
				get_token();
		}
		else
			error("expecting '(' at the start of each condition ", (char *[]){"CBO_TOKEN", "PV_TOKEN", "CBF_TOKEN", "\0"});
	} while ((strcmp(currentToken, "AND_TOKEN") == 0) || (strcmp(currentToken, "OR_TOKEN") == 0));
}
//Loop: for ID in Value { Insts }
//      | for ID in (Num|ID, Num|ID) { Insts }
//      | while (condition ){ Insts }
//      |   do{ Ints }while (condition);
void loop()
{
	//for
	if (strcmp(currentToken, "POUR_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "ID_TOKEN") != 0)
			error("variable d'iteration manquante dans 'pour'", (char *[]){"DANS_TOKEN", "PO_TOKEN", "CBO_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
		if (strcmp(currentToken, "DANS_TOKEN") != 0)
			error("attendant 'dans' après la variable d'itération dans 'pour'", (char *[]){"PO_TOKEN", "CBO_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
		if (strcmp(currentToken, "PO_TOKEN") == 0)
		{
			get_token();
			if (strcmp(currentToken, "NUM_TOKEN") != 0 && strcmp(currentToken, "ID_TOKEN") != 0)
				error("attendant le début de variable d'itération dans 'pour'", (char *[]){"VIR_TOKEN", "PF_TOKEN", "CBO_TOKEN", "CBF_TOKEN", "\0"});
			else
				get_token();
			if (strcmp(currentToken, "VIR_TOKEN") != 0)
				error("attendant ',' dans 'pour' ", (char *[]){"PF_TOKEN", "CBO_TOKEN", "CBF_TOKEN", "\0"});
			else
				get_token();
			if (strcmp(currentToken, "NUM_TOKEN") != 0 && strcmp(currentToken, "ID_TOKEN") != 0)
				error("attendant la fin de variable d'itération dans 'pour'", (char *[]){"PF_TOKEN", "CBO_TOKEN", "CBF_TOKEN", "\0"});
			else
				get_token();
			if (strcmp(currentToken, "PF_TOKEN") != 0)
				error("attendant ')' avant { dans pour", (char *[]){"CBO_TOKEN", "CBF_TOKEN", "\0"});
			else
				get_token();
		}
		else
			is_value();
		if (strcmp(currentToken, "CBO_TOKEN") != 0)
			error("'{' manquant dans 'pour'", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
		Insts();
		if (strcmp(currentToken, "CBF_TOKEN") != 0)
			error("'}' manquant dans 'pour'", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
	} //while
	else if (strcmp(currentToken, "TANTQUE_TOKEN") == 0)
	{
		condition();
		if (strcmp(currentToken, "CBO_TOKEN") != 0)
			error("'{' manquant dans 'tantQue'", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
		Insts();
		if (strcmp(currentToken, "CBF_TOKEN") != 0)
			error("'}' manquant dans 'tantQue'", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
	} //do while
	else if (strcmp(currentToken, "FAIRE_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "CBO_TOKEN") != 0)
			error("'{' manquant dans 'faire-tantQue'", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
		Insts();
		//get_token();
		if (strcmp(currentToken, "CBF_TOKEN") != 0)
			error("'}' manquant dans 'faire-tantQue'", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
		if (strcmp(currentToken, "TANTQUE_TOKEN") != 0)
			error("manquant 'tantQue' dans 'faire-tantQue' ", (char *[]){"PV_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			condition();
		if (strcmp(currentToken, "PV_TOKEN") != 0)
			error("manquant ';' dans la fin de 'faire-tantQue' ", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
	}
}
void decision()
{
	condition();
	if (strcmp(currentToken, "CBO_TOKEN") != 0)
		error("attendant '{' après la condition dans si", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
	else
		get_token();
	Insts();
	if (strcmp(currentToken, "CBF_TOKEN") != 0)
		error("attendant '}' dans la fin de la condition si", (char *[]){"SINONSI_TOKEN", "SINON_TOKEN"
																				   "ID_TOKEN",
																	 "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
	else
		get_token();
	if (strcmp(currentToken, "SINONSI_TOKEN") == 0)
	{
		condition();
		if (strcmp(currentToken, "CBO_TOKEN") != 0)
			error("attendant '{' après condition dans sinonSi", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
		Insts();
		if (strcmp(currentToken, "CBF_TOKEN") != 0)
			error("attendant '}' après condition dans sinonSi", (char *[]){"SINON_TOKEN", "ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
	}
	if (strcmp(currentToken, "SINON_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "CBO_TOKEN") != 0)
			error("attendant '{' après sinon", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
		Insts();
		if (strcmp(currentToken, "CBF_TOKEN") != 0)
			error("attendant '}' à la fin de sinon", (char *[]){"ID_TOKEN", "SI_TOKEN", "POUR_TOKEN", "TANTQUE_TOKEN", "FAIRE_TOKEN", "RETURN_TOKEN", "CBF_TOKEN", "\0"});
		else
			get_token();
	}
}
//EXPR::=TERM { [+|-] TERM } || callFunction || value
void expr()
{
	get_token();
	if (strcmp(currentToken, "ID_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "PO_TOKEN") == 0)
		{
			CallFunction();
		}
		else if (strcmp(currentToken, "BO_TOKEN") == 0)
		{
			get_token();
			if (strcmp(currentToken, "NUM_TOKEN") == 0 || strcmp(currentToken, "ID_TOKEN") == 0)
			{
				get_token();
				if (strcmp(currentToken, "BF_TOKEN") != 0)
					error("attendant ']' après l'indice", (char *[]){"PLUS_TOKEN", "MOINS_TOKEN", "MULT_TOKEN", "MOD_TOKEN", "POWER_TOKEN", "DIV_TOKEN", "\0"});
				else
					get_token();
			}
			else
				error("indice manquant après '['", (char *[]){"PLUS_TOKEN", "MOINS_TOKEN", "MULT_TOKEN", "MOD_TOKEN", "POWER_TOKEN", "DIV_TOKEN", "\0"});
		}
		exprBegin();
	}
	else if (strcmp(currentToken, "NUM_TOKEN") == 0)
	{
		get_token();
		exprBegin();
	}
	else if (strcmp(currentToken, "PO_TOKEN") == 0)
	{
		expr();
		if (strcmp(currentToken, "PF_TOKEN") != 0)
			error("')' manquant dans expression", (char *[]){"PV_TOKEN", "CBF_TOKEN", "\0"});
		else
		{
			get_token();
			exprBegin();
		}
	}
	else
		is_value();
}
//TERM::=FACT {[*|/|^|%] FACT}
void exprBegin()
{
	while ((strcmp(currentToken, "MULT_TOKEN") == 0) || (strcmp(currentToken, "DIV_TOKEN") == 0) || (strcmp(currentToken, "POWER_TOKEN") == 0) || (strcmp(currentToken, "MOD_TOKEN") == 0))
	{
		fact();
	}
	while ((strcmp(currentToken, "PLUS_TOKEN") == 0) || (strcmp(currentToken, "MOINS_TOKEN") == 0))
	{
		term();
	}
}
void term()
{
	do
	{
		fact();
	} while ((strcmp(currentToken, "MULT_TOKEN") == 0) || (strcmp(currentToken, "DIV_TOKEN") == 0) || (strcmp(currentToken, "POWER_TOKEN") == 0) || (strcmp(currentToken, "MOD_TOKEN") == 0));
}
//FACT::=ID | NUM | (EXPR)
void fact()
{
	get_token();
	if (strcmp(currentToken, "ID_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "PO_TOKEN") == 0)
		{
			CallFunction();
		}
		else if (strcmp(currentToken, "BO_TOKEN") == 0)
		{
			get_token();
			if (strcmp(currentToken, "NUM_TOKEN") == 0 || strcmp(currentToken, "ID_TOKEN") == 0)
			{
				get_token();
				if (strcmp(currentToken, "BF_TOKEN") != 0)
					error("attendant ']' après indice", (char *[]){"PLUS_TOKEN", "MOINS_TOKEN", "MULT_TOKEN", "MOD_TOKEN", "POWER_TOKEN", "DIV_TOKEN", "\0"});
				else
					get_token();
			}
			else
				error("indice manquant après '['", (char *[]){"PLUS_TOKEN", "MOINS_TOKEN", "MULT_TOKEN", "MOD_TOKEN", "POWER_TOKEN", "DIV_TOKEN", "\0"});
		}
		return;
	}
	else if (strcmp(currentToken, "PO_TOKEN") == 0)
	{
		expr();
		if (strcmp(currentToken, "PF_TOKEN") != 0)
			error("')' manquant dans expression", (char *[]){"PLUS_TOKEN", "MOINS_TOKEN", "MULT_TOKEN", "MOD_TOKEN", "POWER_TOKEN", "DIV_TOKEN", "\0"});
		else
			get_token();
		return;
	}
	else if (strcmp(currentToken, "NUM_TOKEN") == 0)
	{
		get_token();
		return;
	}
	else
		error("expression non claire", (char *[]){"PV_TOKEN", "PLUS_TOKEN", "MOINS_TOKEN", "MULT_TOKEN", "MOD_TOKEN", "POWER_TOKEN", "DIV_TOKEN", "\0"});
}
void is_value()
{
	//numbers and char
	if (strcmp(currentToken, "NUM_TOKEN") == 0 || strcmp(currentToken, "CHAR_TOKEN") == 0)
	{
		get_token();
	}
	//strings
	else if (strcmp(currentToken, "STRING_TOKEN") == 0)
	{
		get_token();
	}
	//list
	else if (strcmp(currentToken, "LISTE_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "INF_TOKEN") != 0)
			error("attendant '<' après liste", (char *[]){"\0"});
		do
		{
			get_token();
			if (strcmp(currentToken, "ID_TOKEN") == 0)
				get_token();
			else
				is_value();
		} while (strcmp(currentToken, "VIR_TOKEN") == 0);
		if (strcmp(currentToken, "SUP_TOKEN") != 0)
			error("attendant '>' après liste arguments", (char *[]){"PV_TOKEN", "\0"});
		else
			get_token();
	}
	//FILE
	else if (strcmp(currentToken, "FICHIER_TOKEN") == 0)
	{
		get_token();
		if (strcmp(currentToken, "PO_TOKEN") != 0)
			error("attendant '(' après fichier", (char *[]){"STRING_TOKEN", "VIR_TOKEN", "PV_TOKEN", "PF_TOKEN", "\0"});
		else
			get_token();
		if (strcmp(currentToken, "STRING_TOKEN") != 0)
			error("Chemin de fichier manquant", (char *[]){"VIR_TOKEN", "PV_TOKEN", "PF_TOKEN", "\0"});
		else
			get_token();
		if (strcmp(currentToken, "VIR_TOKEN") != 0)
			error("attendant ',' après le répertoire fichier", (char *[]){"CHAR_TOKEN", "PF_TOKEN", "PV_TOKEN", "\0"});
		//accesstype
		else
			get_token();
		if (strcmp(currentToken, "CHAR_TOKEN") != 0)
			error("Type d'accès du fichier manquant", (char *[]){"PF_TOKEN", "PV_TOKEN", "\0"});
		else
			get_token();
		if (strcmp(currentToken, "PF_TOKEN") != 0)
			error("attendant ')' après type d'accès du fichier", (char *[]){"PV_TOKEN", "\0"});
		else
			get_token();
	}
	else
		error("Valeur inconnue", (char *[]){"PV_TOKEN", "\0"});
}
void main(int argc, char *argv[])
{
	// open program from given command argument
	tokens_file = fopen("Tokens", "r");
	//tokens_file = fopen(argv[1], "r");
	error_file = fopen("SyntErrors", "a");
	program();
	fclose(tokens_file);
	fclose(error_file);
}
