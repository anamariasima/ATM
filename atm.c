/* SIMA Ana-Maria - 323CB */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

typedef struct history {
    char result[10];//succes sau nu
    char command[100];//actiunea efectuata
    struct history* next_history;
} *LH, **ALH;

//structura info card
typedef struct {
    char card_number[20];
    char pin[10];
    int modificare_pin; //0 - pin initial, 1 - pin schimbat
    int nr_incercari_pin; //3 incercari
    int inserare; // 1 - inserat, 0 - caz contrar
    char expiry_date[10];
    char cvv[4];
    int balance;
    char status[30];
    LH history;
}info_card;


//lista lsc - o celula este de tip card
typedef struct card {
    info_card info;
    struct card* next_card;
} *LSC, **ALSC;

//lista carduri - o celula este de tip lsc
typedef struct lsc {
    int pos;
    LSC info;
    struct lsc* next_lsc;
} *LC, **ALC;



//aloc celula de tip history
LH AlocCelulaHistory(char* result, char* command) {
    LH aux;
    aux = (LH)malloc(sizeof(struct history));

    if(!aux) {
        return NULL;
    }

    aux->next_history = NULL;
    strcpy(aux->result, result);
    strcpy(aux->command, command);
    return aux;
}

//aloc celula din lsc de tip card
LSC AlocCelulaCard(char* card_number, char* pin, char* expiry_date, char* cvv){
    LSC aux; //celula din lsc
    aux = (LSC)malloc(sizeof(struct card));
    if(!aux) {
        return NULL;
    }
    aux->next_card = NULL;

    strcpy(aux->info.card_number, card_number);
    strcpy(aux->info.pin, pin);
    aux->info.modificare_pin = 0;
    aux->info.nr_incercari_pin = 3;
    aux->info.inserare = 0; // 1- inserare, 0 - deconectare
    strcpy(aux->info.expiry_date, expiry_date);
    strcpy(aux->info.cvv, cvv);
    aux->info.balance = 0;
    strcpy(aux->info.status, "NEW");

    aux->info.history = NULL; 

    return aux;

}

//alocare celula din lc de tip lsc
LC AlocCelulaLSC(char* card_number, char* pin, char* expiry_date, char* cvv){
    LC aux; // celula din lc
    aux = (LC)malloc(sizeof(struct lsc));

    if(!aux) {
        return NULL;
    }
    aux->next_lsc = NULL;
    aux->pos = 0;
    if(strcmp(card_number, " ") == 0) { //daca info card_number este nula, atunci se aloca o celula lsc goala
        aux->info = NULL;

    }
    else { //daca info card_number nu e nula(deci am de adaugat un card), atunci aloc celula de tip card
        aux->info = AlocCelulaCard(card_number, pin, expiry_date, cvv);

    }
    return aux;

}

int InsHistory(LH *a_history, char* result, char* command) {
    LH aux;
    aux = AlocCelulaHistory(result, command);
    if(!aux) {
        return 0;
    }
   
    aux->next_history = *a_history;
    *a_history = aux;
    
    
    return 1;
}

//inserare inceput(inserare card in lsc)
int InsCard(LSC *a_Card, char* card_number, char* pin, char* expiry_date, char* cvv) {
    LSC aux;
    aux = AlocCelulaCard(card_number, pin, expiry_date, cvv);
    if(!aux){
        return 0;
    }
    aux->next_card = *a_Card;
    *a_Card = aux;
    return 1;
}

//inserare inceput in lista lc - inserare de submultime de carduri
int InsIncLSC(LC *a_LSC, char* card_number, char* pin, char* expiry_date, char* cvv, int pos) {
    LC aux;
    aux = AlocCelulaLSC(card_number, pin, expiry_date, cvv);
    aux->pos = pos;
    if(!aux){
        return 0;
    }
    if((*a_LSC) == NULL) {
        *a_LSC = aux;
    }
    else {
        aux->next_lsc = *a_LSC;
        *a_LSC = aux;

    }
    return 1;
}



//calculeaza pozitie card in lc
int CalcPos(int nr_max_card, char* card_number) {
    int i, sum = 0, pos;
    for(i = 0; i < strlen(card_number); i++) {
        sum += card_number[i] - '0';

    }

    pos = sum % nr_max_card;
    return pos;
}

//afisare lista history
void AfisareHistory(FILE* fout, LH history) {
    if(!history) {
        fprintf(fout, "history: []");
    }
    else {
        fprintf(fout, "history: [");
        fprintf(fout, "(%s", history->result);
        fprintf(fout, ", ");
        fprintf(fout, "%s", history->command);
        fprintf(fout, ")");
        history = history->next_history;
        while (history) {
            // printf("history: [(");
            fprintf(fout, ", ");
            fprintf(fout, "(%s", history->result);
            fprintf(fout, ", ");
            fprintf(fout, "%s", history->command);
            fprintf(fout, ")");
            history = history->next_history;
        }
        fprintf(fout, "]");

    }
}
//afisare lista lc
void AfisareLC(FILE* fout, LC a_LSC, int max_pos) {
    //int nr = 0;
    //LC aux_lsc;
    LSC aux_card = NULL;
    //aux_lsc = a_LSC;
    while (a_LSC) {
        if (max_pos >= 0) {

            if (!(a_LSC->info)) {

                fprintf(fout, "pos%d: []", a_LSC->pos);
            }
            else {
                fprintf(fout, "pos%d: [", a_LSC->pos);
                for (aux_card = a_LSC->info; aux_card!= NULL; aux_card = aux_card->next_card) {

                    fprintf(fout, "\n(card number: %s", aux_card->info.card_number);
                    fprintf(fout, ", ");
                    fprintf(fout, "PIN: %s", aux_card->info.pin);
                    fprintf(fout, ", ");
                    fprintf(fout, "expiry date: %s", aux_card->info.expiry_date);
                    fprintf(fout, ", ");
                    fprintf(fout, "CVV: %s", aux_card->info.cvv);
                    fprintf(fout, ", ");
                    fprintf(fout, "balance: %d", aux_card->info.balance);
                    fprintf(fout, ", ");
                    fprintf(fout, "status: %s", aux_card->info.status);
                    fprintf(fout, ", ");


                    AfisareHistory(fout, aux_card->info.history);
                    // printf(")");
                    fprintf(fout, ")");


                }
                fprintf(fout, "\n]");
            }
            fprintf(fout, "\n");

            a_LSC = a_LSC->next_lsc;
            max_pos--;
        }
        else
            break;

       
    }

}


//verific daca am cardul in lista
int ExistaCard(LSC a_Card, char* card_number) {
    LSC aux_card;
    for(aux_card = a_Card; aux_card != NULL; aux_card = aux_card->next_card) {
        if(strcmp(aux_card->info.card_number, card_number) == 0) {
            return 1;
        }

    }
    return 0;

}

//verific daca am alocata o lsc pe pozitia pos
int ExistaPozitie(LC a_LSC, int pos) {
    LC aux_lsc;
    for(aux_lsc = a_LSC; aux_lsc != NULL; aux_lsc = aux_lsc->next_lsc) {
        if(aux_lsc->pos == pos) {
            return 1;
        }
    }
    return 0;
}
//exista pozitia pos in lista lc
/*LSC RetPozitie(LC a_LSC, int pos) {
    LC aux_LSC;
    for(aux_LSC = a_LSC; aux_LSC != NULL; aux_LSC = aux_LSC->next_lsc) {
        if(aux_LSC->pos == pos) {
            return aux_LSC->info;
        }
    }
    return NULL;
}*/

//elimina element(celula) de tip history
int EliminaHistory(ALH a_history, char* command) {
    LH ant, p;
    for(p = *a_history, ant = NULL; p != NULL; ant = p, p = p->next_history) {
        if(strcmp(p->command, command) == 0) {
            break;
        }
    }
    if(p == NULL) {
        return 0;
    }
    if(ant == NULL) {
        *a_history = p->next_history;
    }
    else {
        ant->next_history = p->next_history;
    }

    free(p);
    return 1;

}
//elimina element de tip card
int EliminaCard(ALSC a_Card, char* card_number) {
    LSC ant, p;
    for(p = *a_Card, ant = NULL; p != NULL; ant = p, p = p->next_card) {
        if(strcmp(p->info.card_number, card_number) == 0) {
            break;
        }
    }
    if(p == NULL) {
        return 0;
    }
    if(ant == NULL) {
        *a_Card = p->next_card;
    }
    else {
        ant->next_card = p->next_card;
    }

    free(p);
    return 1;

}

//deblocheaza un card(unblock card)
void DeblocareCard(LSC *a_Card, char* card_number) {

    LSC aux_card = NULL;
    for(aux_card = *a_Card; aux_card != NULL; aux_card = aux_card->next_card) {
        if(strcmp(aux_card->info.card_number, card_number) == 0) {
            if(strcmp((*a_Card)->info.status, "LOCKED") != 0) {
                strcpy((*a_Card)->info.status, "ACTIVE");

            }
            else {
                strcpy((*a_Card)->info.status, "ACTIVE");
            }
            break;
        }
    }
}

//copiaza o lista de tip history
LH Copie(LH a_history) { //copiez fiecare element in noua lista pe noua poz 
    LH nou, aux, u;
    nou = NULL;
    while(a_history) {

        aux = AlocCelulaHistory(a_history->result, a_history->command);
        if(!aux) {
            return NULL;
        }
        if(nou == NULL) {
            nou = aux;

        }
        else {
            u->next_history = aux;

        }
        u = aux;
        a_history = a_history->next_history;
    }
    return nou;
}

//redimensionare lista (nr curent carduri egal cu nr max carduri)
LC RedLC(LC a_LSC, int nr_max_card) {
    int i;

    LC SubCarduriRed = NULL;
    for(i = 15 ; i >= 0; i--) {
        InsIncLSC(&SubCarduriRed, " ", NULL, NULL, NULL, i);

    }
    LC aux_lsc = NULL;
    LC aux_lsc2 = NULL;
     for(aux_lsc = SubCarduriRed; aux_lsc != NULL; aux_lsc = aux_lsc->next_lsc) {
        LC aux_lsc2 = NULL;
        for(aux_lsc2 = a_LSC; aux_lsc2 != NULL; aux_lsc2 = aux_lsc2 ->next_lsc ) {
            LSC aux_card = NULL;
            for(aux_card = aux_lsc2->info; aux_card != NULL; aux_card= aux_card->next_card) {
                int pos = 0;
                pos = CalcPos(nr_max_card, aux_card->info.card_number);
                
                    if(pos == aux_lsc->pos) {
                        if(aux_lsc->info != NULL) {
                            InsCard(&(aux_lsc->info), aux_card->info.card_number, aux_card->info.pin, aux_card->info.expiry_date, aux_card->info.cvv);
                            aux_lsc->info->info.history = Copie(aux_card->info.history);
                        //aux_lsc->info->next_card = AlocCelulaCard(aux_card->info.card_number, aux_card->info.pin, aux_card->info.expiry_date, aux_card->info.cvv);
                        }
                        else
                            aux_lsc->info = AlocCelulaCard(aux_card->info.card_number, aux_card->info.pin, aux_card->info.expiry_date, aux_card->info.cvv);
                            aux_lsc->info->info.history = Copie(aux_card->info.history);
                    }
               
            }
        }
    }
    return SubCarduriRed;
}


//depunere suma sum pe card(recharge)
int Depunere(LSC *a_Card, int sum) {
    if(sum % 10 == 0) {
        (*a_Card)->info.balance += sum;
        return 1;
    }

    return 0;

}

//retragere sum de pe card(cash withdral)
int Retragere(LSC *a_Card, int sum) { // -1 insufficient funds
    if(sum % 10 == 0) {
        if((*a_Card)->info.balance >= sum) {
            (*a_Card)->info.balance -= sum;
            return 1;
        }

        return -1;

    }
    return 0;

}

int main(int argc, char** argv) {

    FILE* fin;
    FILE* fout;
    int max_pos = -1;
    LC SubCarduri = NULL;
    int i, j, pos = 0;
    int nr_curent_carduri = 0;
    char card_number[20];
    char pin[5];
    char expiry_date[10];
    char cvv[4];
    int balance;
    char status[30];
    fin = fopen("input.in", "r");
    if(!fin)
        printf("error");
    fout = fopen("output.out", "w+");
    if(!fout)
         printf("error");
    char buff[1000];
    char *token;
    fgets(buff, sizeof buff, fin);
    int nr_max_card;
    nr_max_card = atoi(buff);
    printf("%d\n", nr_max_card);

    //inserare celule lsc goale
    for(i = 15 ; i >= 0; i--) {
        InsIncLSC(&SubCarduri," ", NULL, NULL, NULL, i);

    }
    

    
    while (fgets(buff, sizeof buff, fin) != NULL) {
        token = strtok(buff," \n\t");


        
        //functia de add
        if(strcmp(token, "add_card") == 0) {

            token = strtok(NULL," \n\t");
            strcpy(card_number, token);
            
            token = strtok(NULL," \n\t");
            strcpy(pin, token);
            
            token = strtok(NULL," \n\t");
            strcpy(expiry_date, token);
            
            token = strtok(NULL," \n\t");
            strcpy(cvv, token);
            

            LC aux_SubCarduri = NULL;
            
            
             //redimensionare
             if(nr_curent_carduri == nr_max_card) {
                nr_max_card = 2 * nr_max_card;
                 SubCarduri = RedLC(SubCarduri, nr_max_card);
            }//final redimensionare

             pos = CalcPos(nr_max_card, card_number);
           
             if (pos > max_pos) {
                                max_pos = pos;
                                printf("%d", max_pos);
                            } 

            if(ExistaPozitie(SubCarduri, pos) == 1) {
             
                for(aux_SubCarduri = SubCarduri; aux_SubCarduri != NULL; aux_SubCarduri = aux_SubCarduri->next_lsc) {
                    if(aux_SubCarduri->pos == pos) {


                        if(aux_SubCarduri->info != NULL) {
                        
                            if(ExistaCard(aux_SubCarduri->info, card_number) == 1) {
                               
                                fprintf(fout, "The card already exists\n");
                            }
                            else {
                                if(InsCard(&(aux_SubCarduri->info), card_number, pin, expiry_date, cvv)) {
                                    nr_curent_carduri++;
                                    if (pos > max_pos) {
                                        max_pos = pos;
                                        
                                    } 
                                    //printf("c");
                                }
                            }


                        }

                        else {

                            aux_SubCarduri->info = AlocCelulaCard(card_number, pin, expiry_date, cvv);


                            nr_curent_carduri++;
                            if (pos > max_pos) {
                                max_pos = pos;
                               
                            } 

                        }
                        break;

                    }


                }

            }

           
             

        }  //final add



            //functia de delete
        else if(strcmp(token, "delete_card") == 0) {
            token = strtok(NULL," \n\t");
            strcpy(card_number, token);
            LC aux_SubCarduri = NULL;
            pos = CalcPos(nr_max_card, card_number);
            if(ExistaPozitie(SubCarduri, pos) == 1) {
                for(aux_SubCarduri = SubCarduri; aux_SubCarduri != NULL; aux_SubCarduri = aux_SubCarduri->next_lsc) {
                    if(aux_SubCarduri->pos == pos) {
                        if(aux_SubCarduri->info != NULL) {
                            if(ExistaCard(aux_SubCarduri->info, card_number) == 1) {
                                EliminaCard(&(aux_SubCarduri->info), card_number);
                                nr_curent_carduri--;

                            }
                        }
                        break;
                    }
                }
            }
        }//final delete

            //functia de unblock
        else if(strcmp(token, "unblock_card") == 0) {
            LC aux_SubCarduri = NULL;
            token = strtok(NULL," \n\t");
            strcpy(card_number, token);
            pos = CalcPos(nr_max_card, card_number);
            if(ExistaPozitie(SubCarduri, pos) == 1) {
                for(aux_SubCarduri = SubCarduri; aux_SubCarduri != NULL; aux_SubCarduri = aux_SubCarduri->next_lsc) {
                    if(aux_SubCarduri->pos == pos) {
                        if(aux_SubCarduri->info != NULL) {
                            if(ExistaCard(aux_SubCarduri->info, card_number) == 1) {
                                DeblocareCard(&(aux_SubCarduri->info), card_number);
                                aux_SubCarduri->info->info.nr_incercari_pin = 3;
                            }
                        }
                        break;
                    }
                }
               
            }
        }
        
        //anulare tranzactie
        else if(strcmp(token, "reverse_transaction") == 0) {
            LC aux_lsc1 = NULL;
            LSC aux_card1= NULL;
            LC aux_lsc2 = NULL;
            LSC aux_card2= NULL;
            char card_number1[20];
            char card_number2[20];
            int pos1, pos2;
            char command[50];
            int sum;
           
            strcpy(command, token);
            token = strtok(NULL, " \n\t");
            strcpy(card_number1, token);
            strcat(command, " ");
            strcat(command, token);
            token = strtok(NULL, " \n\t");
            strcpy(card_number2, token);
            strcat(command, " ");
            strcat(command, token);
            token = strtok(NULL, " \n\t");
            sum = atoi(token);
            strcat(command, " ");
            strcat(command, token);
           
            pos1 = CalcPos(nr_max_card, card_number1);
            pos2 = CalcPos(nr_max_card, card_number2);
            for(aux_lsc1 = SubCarduri; aux_lsc1 != NULL; aux_lsc1 = aux_lsc1->next_lsc) { //parcurg lc - gasesc lsc pos
                if (aux_lsc1->pos == pos1) {
                    for (aux_card1 = aux_lsc1->info; aux_card1 != NULL; aux_card1 = aux_card1->next_card) {
                        if (strcmp(aux_card1->info.card_number, card_number1) == 0) {
                           
                                for(aux_lsc2 = SubCarduri; aux_lsc2 != NULL; aux_lsc2 = aux_lsc2->next_lsc) {
                                    if (aux_lsc2->pos == pos2) {
                                        for (aux_card2 = aux_lsc2->info; aux_card2 != NULL; aux_card2 = aux_card2->next_card) {
                                            if (strcmp(aux_card2->info.card_number, card_number2) == 0) {
                                              
                                                    

                                                    if(Retragere(&(aux_card2), sum) == 1) {
                                                        if(Depunere(&(aux_card1), sum) == 1) {
                                                        
                                                            InsHistory(&(aux_card1->info.history), "SUCCESS", command); 
                                                            char removed_command[50];
                                                            strcpy(removed_command, "transfer_funds");
                                                            strcat(removed_command, " ");
                                                            strcat(removed_command, card_number1);
                                                            strcat(removed_command, " ");
                                                            strcat(removed_command, card_number2);
                                                            strcat(removed_command, " ");
                                                            strcat(removed_command, token);
                                                            EliminaHistory(&(aux_card2->info.history), removed_command);
                                                        
                                                        }
                                                    }
                                        
                                                    else if(sum % 10 != 0) {
                                                        fprintf(fout, "The transferred amount must be multiple of 10\n");
                                                       
                                                    }
                                                    else if(aux_card2->info.balance < sum) {
                                                        fprintf(fout, "The transaction cannot be reversed\n");
                                                        
                                                    }
                                                
                                            
                                                break;

                                            }
                                        }
                                        break;
                                    }    
                               // }
                            
                                }
                         
                            break;
                        }
                       
                    }
                    break;
                }
            }
            
        }
        //final anulare tranzactie


            //functia de show
        else if(strcmp(token, "show") == 0) {
            token = strtok(NULL," \n\t");
          
            if(token) {
                strcpy(card_number, token);
                LC aux_SubCarduri = NULL;
                LSC aux_Carduri = NULL;
                pos = CalcPos(nr_max_card, card_number);
                for(aux_SubCarduri = SubCarduri; aux_SubCarduri != NULL; aux_SubCarduri = aux_SubCarduri->next_lsc) {
                    if(aux_SubCarduri->pos == pos) {
                        for(aux_Carduri = aux_SubCarduri->info; aux_Carduri != NULL; aux_Carduri = aux_Carduri->next_card) {
                            if(strcmp(aux_Carduri->info.card_number, card_number) == 0) {
                                fprintf(fout, "(card number: %s", aux_Carduri->info.card_number);
                                fprintf(fout, ", ");
                                fprintf(fout, "PIN: %s", aux_Carduri->info.pin);
                                fprintf(fout, ", ");
                                fprintf(fout, "expiry date: %s", aux_Carduri->info.expiry_date);
                                fprintf(fout, ", ");
                                fprintf(fout, "CVV: %s", aux_Carduri->info.cvv);
                                fprintf(fout, ", ");
                                fprintf(fout, "balance: %d", aux_Carduri->info.balance);
                                fprintf(fout, ", ");
                                fprintf(fout, "status: %s", aux_Carduri->info.status);
                                fprintf(fout, ", ");
                               
                                    AfisareHistory(fout, aux_Carduri->info.history);
                                    fprintf(fout, ")\n");
                              
                                break;

                            }
                        }

                        
                        break;
                    }
                }
            }
            else {
                AfisareLC(fout, SubCarduri, max_pos);
            }
        } //final show



        //insert
        
        else if(strcmp(token, "insert_card") == 0) {
            LC aux_lsc = NULL;
            LSC aux_card = NULL;
            char command[50];
            strcpy(command, token);
            token = strtok(NULL," \n\t");
            strcpy(card_number, token);
            strcat(command, " ");
            strcat(command, token);
            token = strtok(NULL," \n\t");
           
           
                strcpy(pin, token);
                strcat(command, " ");
                strcat(command, token);
                pos = CalcPos(nr_max_card, card_number);
                for(aux_lsc = SubCarduri; aux_lsc != NULL; aux_lsc = aux_lsc->next_lsc) { //parcurg lc - gasesc lsc pos
                    if(aux_lsc->pos == pos) {
                        for(aux_card = aux_lsc->info; aux_card != NULL; aux_card = aux_card->next_card) { //parcurc lsc - gasesc card card_number
                            if(strcmp(aux_card->info.card_number, card_number) == 0) {
                                if(strcmp(aux_card->info.status, "LOCKED") != 0) {
                                    
                                    if(strlen(pin) != 4) {
                                        
                                        if(aux_card->info.nr_incercari_pin <= 1) {
                                            fprintf(fout, "Invalid PIN\n");
                                            fprintf(fout, "The card is blocked. Please contact the administrator.\n");
                                            strcpy(aux_card->info.status, "LOCKED");
                                    
                                        }
                                        else {
                                            fprintf(fout, "Invalid PIN\n");
                                            
                                        }
                                        
                                        InsHistory(&(aux_card->info.history), "FAIL", command);
                                        aux_card->info.nr_incercari_pin--;
                                    }   
                                    else if(strcmp(aux_card->info.pin, pin) != 0) {
                                        
                                        
                                        if(aux_card->info.nr_incercari_pin <= 1) {
                                            fprintf(fout, "Invalid PIN\n");
                                            fprintf(fout, "The card is blocked. Please contact the administrator.\n");
                                            strcpy(aux_card->info.status, "LOCKED");
                                    
                                        }
                                        else {
                                            fprintf(fout, "Invalid PIN\n");
                                          
                                        }
                                        
                                        InsHistory(&(aux_card->info.history), "FAIL", command);
                                        aux_card->info.nr_incercari_pin--;
                                    }   
                                    else if(aux_card->info.modificare_pin == 0) {
                                        if(aux_card->info.inserare == 0) {
                                            fprintf(fout, "You must change your PIN.\n" );
                                            
                                            strcpy(aux_card->info.status, "NEW");
                                            InsHistory(&(aux_card->info.history), "SUCCESS", command);
                                        }
                                        else {
                                            fprintf(fout, "You must change your PIN.\n" );
                                          
                                            strcpy(aux_card->info.status, "ACTIVE");
                                            InsHistory(&(aux_card->info.history), "SUCCESS", command);
                                        }
                                        aux_card->info.inserare = 1; 
                                    }
                                    else if(aux_card->info.modificare_pin == 1) {
                                         
                                        strcpy(aux_card->info.status, "ACTIVE");
                                        InsHistory(&(aux_card->info.history), "SUCCESS", command);
                                        aux_card->info.inserare = 1; 
                                    }
                    
                                }
                                else {
                                    fprintf(fout, "The card is blocked. Please contact the administrator.\n");
                                    InsHistory(&(aux_card->info.history), "FAIL", command);
                                }
                                break;
                                
                                
                            }
                        }
                        break;
                    }
                }
            
            
        }

        //final insert
        
            //functia de depunere
       else if(strcmp(token, "recharge") == 0) {
            LC aux_lsc = NULL;
            LSC aux_card = NULL;
            int sum = 0;
            char command[100];
            char result[10];
            strcpy(command, token);
            token = strtok(NULL, " \n\t");
            strcpy(card_number, token);
            strcat(command, " ");
            strcat(command, token);
            token = strtok(NULL, " \n\t");
            sum = atoi(token);
            strcat(command, " ");
            strcat(command, token);
          
            pos = CalcPos(nr_max_card, card_number);
            for(aux_lsc = SubCarduri; aux_lsc != NULL; aux_lsc = aux_lsc->next_lsc) { //parcurg lc - gasesc lsc pos
                if(aux_lsc->pos == pos) {
                    for(aux_card = aux_lsc->info; aux_card != NULL; aux_card = aux_card->next_card) { //parcurc lsc - gasesc card card_number
                        if(strcmp(aux_card->info.card_number, card_number) == 0) {
                            if(aux_card->info.inserare == 1 && strcmp(aux_card->info.status, "LOCKED") != 0) {
                                if(Depunere(&(aux_card), sum)) {
                                    fprintf(fout, "%d\n", aux_card->info.balance);
                                
                                
                                    strcpy(aux_card->info.status, "NEW");
                                    InsHistory(&(aux_card->info.history), "SUCCESS", command);
                                
                                
                                }
                                else {
                                    fprintf(fout, "The added amount must be multiple of 10\n");
                                    InsHistory(&(aux_card->info.history), "FAIL", command);
                                }
                           
                            }
                            else {
                                if(strcmp(aux_card->info.status, "LOCKED") == 0) {
                                    fprintf(fout, "The card is blocked. Please contact the administrator.\n");
                                }
                                InsHistory(&(aux_card->info.history), "FAIL", command);
                            }
                        }
                        break;
                    }
                    break;
                }

            }


        }

            //final depunere
            

            //retragere
        else if(strcmp(token, "cash_withdrawal") == 0) {
            LC aux_lsc = NULL;
            LSC aux_card = NULL;
            int sum;
            char command[50];
            strcpy(command, token);
            token = strtok(NULL, " \n\t");
            strcpy(card_number, token);
            strcat(command, " ");
            strcat(command, token);
            token = strtok(NULL, " \n\t");
            sum = atoi(token);
            strcat(command, " ");
            strcat(command, token);
           
            pos = CalcPos(nr_max_card, card_number);
            for(aux_lsc = SubCarduri; aux_lsc != NULL; aux_lsc = aux_lsc->next_lsc) { //parcurg lc - gasesc lsc pos
                if (aux_lsc->pos == pos) {
                    for (aux_card = aux_lsc->info; aux_card != NULL; aux_card = aux_card->next_card) {
                        if (strcmp(aux_card->info.card_number, card_number) == 0) {
                            if(aux_card->info.inserare == 1 && strcmp(aux_card->info.status, "LOCKED") != 0) {
                                if (Retragere(&(aux_card), sum) == 1) {
                                    fprintf(fout, "%d\n", aux_card->info.balance);

                                    strcpy(aux_card->info.status, "NEW");
                                    InsHistory(&(aux_card->info.history), "SUCCESS", command);

                                }   

                                else if (Retragere(&(aux_card), sum) == -1) {
                                        fprintf(fout, "Insufficient funds\n");
                                        InsHistory(&(aux_card->info.history), "FAIL", command);
                                } 
                                else {
                                    fprintf(fout, "The requested amount must be multiple of 10\n");
                                    InsHistory(&(aux_card->info.history), "FAIL", command);
                                }
                            }
                            else {
                                if(strcmp(aux_card->info.status, "LOCKED") == 0) {
                                    fprintf(fout, "The card is blocked. Please contact the administrator.\n");
                                }
                                InsHistory(&(aux_card->info.history), "FAIL", command);
                            }
                            break;
                        }
                    }
                    break;
                }
            }
        }
            //final retragere

        
            //balance
        else if(strcmp(token, "balance_inquiry") == 0) {
            LC aux_lsc = NULL;
            LSC aux_card = NULL;
            char command[50];
            strcpy(command, token);
            token = strtok(NULL, " \n\t");
            strcpy(card_number, token);
            strcat(command, " ");
            strcat(command, token);
            pos = CalcPos(nr_max_card, card_number);
            for(aux_lsc = SubCarduri; aux_lsc != NULL; aux_lsc = aux_lsc->next_lsc) { //parcurg lc - gasesc lsc pos
                if (aux_lsc->pos == pos) {
                    for (aux_card = aux_lsc->info; aux_card != NULL; aux_card = aux_card->next_card) {
                        if (strcmp(aux_card->info.card_number, card_number) == 0) {
                            if(aux_card->info.inserare == 1 && strcmp(aux_card->info.status, "LOCKED") != 0) {
                                fprintf(fout, "%d\n", aux_card->info.balance);

                                strcpy(aux_card->info.status, "NEW");
                                InsHistory(&(aux_card->info.history), "SUCCESS", command);
                               
                            }
                            else {
                                if(strcmp(aux_card->info.status, "LOCKED") == 0) {
                                    fprintf(fout, "The card is blocked. Please contact the administrator.\n");
                                }
                                InsHistory(&(aux_card->info.history), "FAIL", command);
                                

                            }
                            break;
                        }
                        
                    }
                    break;
                }
            }
        }
        //final balance

        //tranzactie
        else if(strcmp(token, "transfer_funds") == 0) {
            LC aux_lsc1 = NULL;
            LSC aux_card1= NULL;
            LC aux_lsc2 = NULL;
            LSC aux_card2= NULL;
            char card_number1[20];
            char card_number2[20];
            int pos1, pos2;
            char command[50];
            int sum;
            strcpy(command, token);
            token = strtok(NULL, " \n\t");
            strcpy(card_number1, token);
            strcat(command, " ");
            strcat(command, token);
            token = strtok(NULL, " \n\t");
            strcpy(card_number2, token);
            strcat(command, " ");
            strcat(command, token);
            token = strtok(NULL, " \n\t");
            sum = atoi(token);
            strcat(command, " ");
            strcat(command, token);
            printf("%s\n", command);
            pos1 = CalcPos(nr_max_card, card_number1);
            pos2 = CalcPos(nr_max_card, card_number2);
            for(aux_lsc1 = SubCarduri; aux_lsc1 != NULL; aux_lsc1 = aux_lsc1->next_lsc) { //parcurg lc - gasesc lsc pos
                if (aux_lsc1->pos == pos1) {
                    for (aux_card1 = aux_lsc1->info; aux_card1 != NULL; aux_card1 = aux_card1->next_card) {
                        if (strcmp(aux_card1->info.card_number, card_number1) == 0) {
                            if(aux_card1->info.inserare == 1 && strcmp(aux_card1->info.status, "LOCKED") != 0) {
                                for(aux_lsc2 = SubCarduri; aux_lsc2 != NULL; aux_lsc2 = aux_lsc2->next_lsc) {
                                    if (aux_lsc2->pos == pos2) {
                                        for (aux_card2 = aux_lsc2->info; aux_card2 != NULL; aux_card2 = aux_card2->next_card) {
                                            if (strcmp(aux_card2->info.card_number, card_number2) == 0) {
                                               // if(aux_card2->info.inserare == 1 && strcmp(aux_card2->info.status, "LOCKED") != 0) {
                                                    printf("intra");

                                                    if(Retragere(&(aux_card1), sum) == 1) {
                                                        if(Depunere(&(aux_card2), sum) == 1) {
                                                            fprintf(fout, "%d\n", aux_card1->info.balance);
                                                            InsHistory(&(aux_card1->info.history), "SUCCESS", command);
                                                            InsHistory(&(aux_card2->info.history), "SUCCESS", command);
                                                        }
                                                    }
                                        
                                                    else if(Retragere(&(aux_card1), sum) == 0) {
                                                        fprintf(fout, "The transferred amount must be multiple of 10\n");
                                                        InsHistory(&(aux_card1->info.history), "FAIL", command);
                                                    }
                                                    else if(Retragere(&(aux_card1), sum) == -1) {
                                                        fprintf(fout, "Insufficient funds\n");
                                                        InsHistory(&(aux_card1->info.history), "FAIL", command);
                                                    }
                                                
                                            
                                                break;

                                            }
                                        }
                                        break;
                                    }    
                                }
                            
                            }
                            else {
                               
                                if(strcmp(aux_card1->info.status, "LOCKED") == 0) {
                                    fprintf(fout, "The card is blocked. Please contact the administrator.\n");
                                }
                                InsHistory(&(aux_card1->info.history), "FAIL", command);
                                

                            }
                            break;
                        }
                       
                    }
                    break;
                }
            }
            strcpy(aux_card1->info.status, "NEW");
            strcpy(aux_card2->info.status, "NEW");
        }

        //final tranzactie
        
        //schimbare pin
        else if(strcmp(token, "pin_change") == 0) {
            LC aux_lsc = NULL;
            LSC aux_card = NULL;
            char command[50];
            strcpy(command, token);
            token = strtok(NULL, " \n\t");
            strcpy(card_number, token);
            strcat(command, " ");
            strcat(command, token);
            token = strtok(NULL, " \n\t");
            strcpy(pin, token);
            strcat(command, " ");
            strcat(command, token);
            pos = CalcPos(nr_max_card, card_number);
            for(aux_lsc = SubCarduri; aux_lsc != NULL; aux_lsc = aux_lsc->next_lsc) { //parcurg lc - gasesc lsc pos
                if (aux_lsc->pos == pos) {
                    for (aux_card = aux_lsc->info; aux_card != NULL; aux_card = aux_card->next_card) {
                        if (strcmp(aux_card->info.card_number, card_number) == 0) {
                            if(aux_card->info.inserare == 1 && strcmp(aux_card->info.status, "LOCKED") != 0) {
                                   if(strlen(pin) == 4) {
                                    strcpy(aux_card->info.pin, pin);
                                    aux_card->info.modificare_pin = 1;
                                    strcpy(aux_card->info.status, "ACTIVE");
                                    InsHistory(&(aux_card->info.history), "SUCCESS", command);

                                    
                                    }
                                    else {
                                        fprintf(fout, "%s\n", "Invalid PIN");
                                        InsHistory(&(aux_card->info.history), "FAIL", command);
                                      
                                    }

                            }
                            else {
                                if(strcmp(aux_card->info.status, "LOCKED") == 0) {
                                fprintf(fout, "The card is blocked. Please contact the administrator.\n");
                                
                                }
                                InsHistory(&(aux_card->info.history), "FAIL", command);
                               
                            }
                            break;
                        }
                       
                    }
                    break;
                }
            }
        }

        //final schimbare pin

        //cancel
        else if(strcmp(token, "cancel") == 0) {
            LC aux_lsc = NULL;
            LSC aux_card = NULL;
            char command[50];
            strcpy(command, token);
            token = strtok(NULL, " \n\t");
            strcpy(card_number, token);
            strcat(command, " ");
            strcat(command, token);
            pos = CalcPos(nr_max_card, card_number);
            for(aux_lsc = SubCarduri; aux_lsc != NULL; aux_lsc = aux_lsc->next_lsc) { //parcurg lc - gasesc lsc pos
                if (aux_lsc->pos == pos) {
                    for (aux_card = aux_lsc->info; aux_card != NULL; aux_card = aux_card->next_card) {
                        if (strcmp(aux_card->info.card_number, card_number) == 0) {
                            if(aux_card->info.inserare == 1 && strcmp(aux_card->info.status, "LOCKED") != 0) {
                                aux_card->info.inserare = 0;
                                aux_card->info.nr_incercari_pin = 3;
                                InsHistory(&(aux_card->info.history), "SUCCESS", command);
                                break;
                            }
                            else {
                                if(strcmp(aux_card->info.status, "LOCKED") == 0) {
                                     
                                    fprintf(fout, "The card is blocked. Please contact the administrator.\n");
                                }
                                InsHistory(&(aux_card->info.history), "FAIL", command);
                                break;
                            }
                            strcpy(aux_card->info.status, "ACTIVE");

                        }
                        
                    }
                    break;
                }
            }
        }
        //final cancel

    }//sfarsit while
    
    fclose(fin);
    fclose(fout);


    return 0;
}//sfarsit main
