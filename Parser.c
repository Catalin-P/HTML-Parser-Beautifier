#include "lib.h"

/*
 *  Pentru a ușura citirea codului HTML din fișier, vi se pune la dispoziție
 *  următoarea funcție. Aceasta primeste un singur caracter (fișierul va fi
 *  citit caracter cu caracter în afara acestei funcții, iar ea va fi apelată
 *  de fiecare dată) și, în funcție de caracter, trece dintr-o stare în alta.
 *  Comentariile din cod arată o sugestie (neobligatorie) despre cum ar trebui
 *  completată funcția pentru a construi arborele. 
 * 
 *  Este, cu alte cuvinte, un automat de stări, unde starea inițială este PARSE_CONTENTS.
 */

TParseState Interpret(TParseState currentState, char c, void* ancestorStack, TArb *crtNode, char* name, char* value)
{
    TParseState nextState = PARSE_ERROR;

    switch (currentState)
    {
    case PARSE_CONTENTS:
        if (c == '<')           {   nextState = PARSE_OPENING_BRACKET;  }
        else                    {   nextState = PARSE_CONTENTS; 
                                    /* TODO append to `info->contents` string */ 
                                    appendToContent(*crtNode, c);
                                    } 
        break;
    case PARSE_OPENING_BRACKET:
        if (isspace(c))         {   nextState = PARSE_OPENING_BRACKET;  }
        else if (c == '>')      {   nextState = PARSE_ERROR;            }
        else if (c == '/')      {   nextState = PARSE_CLOSING_TAG;      }
        else                    {   nextState = PARSE_TAG_TYPE; 
                                    /* TODO create tag node */ 
                                     *crtNode=allocTreeCell();
                                     appendToTagType(*crtNode, c);
                                     Push(ancestorStack, *crtNode);
                                    }
        break;
    case PARSE_TAG_TYPE:
        if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
        else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
        else                    {   nextState = PARSE_TAG_TYPE; 
                                    /* TODO append to `info->type` string */ 
                                    appendToTagType(*crtNode, c);       }
        break;
    case PARSE_CLOSING_TAG:
        if (c == '>')           {   nextState = PARSE_CONTENTS; 
                                    /* TODO insert tag node in `children` list */ 
                                    insertInChildrenList(ancestorStack);}
        else                    {   nextState = PARSE_CLOSING_TAG;      }
        break;
    case PARSE_REST_OF_TAG:
        if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
        else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
        else if (c == '/')      {   nextState = PARSE_SELF_CLOSING;     }
        else                    {   nextState = PARSE_ATTRIBUTE_NAME;   
                                    appendToAttribute(name, c);         }
        break;
    case PARSE_ATTRIBUTE_NAME:
        if (c == '=')           {   nextState = PARSE_ATTRIBUTE_EQ;     }
        else                    {   nextState = PARSE_ATTRIBUTE_NAME; 
                                    /* TODO append to a temporary "name" string */ 
                                    appendToAttribute(name, c);         
                                    }
        break;
    case PARSE_ATTRIBUTE_EQ:
        if (c == '\"')          {   nextState = PARSE_ATTRIBUTE_VALUE;  }
        break;
    case PARSE_ATTRIBUTE_VALUE:
        if (c == '\"')          {   nextState = PARSE_REST_OF_TAG; 
                                    /* TODO create attribute from the temporary strings*/ 
                                    insertAttributeCell(*crtNode, name, value);       }
        else                    {   nextState = PARSE_ATTRIBUTE_VALUE; 
                                    /* TODO append to a temporary "value" string */ 
                                    appendToAttribute(value, c);            
                                   }
        break;
    case PARSE_SELF_CLOSING:
        if (c == '>')           {   nextState = PARSE_CONTENTS; 
                                    /* TODO insert self-closing tag node in `children` list*/
                                    (*crtNode)->info->isSelfClosing=1;
                                    insertInChildrenList(ancestorStack);    }
        else                    {   nextState = PARSE_ERROR;                }
        break;
    default:
        break;
    }

    /* For debugging */
#if 0
    printf("%d", nextState);
    if (c == '\n') printf("\n");
#endif

    return nextState;
}