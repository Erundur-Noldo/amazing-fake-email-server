#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h> /*-lncurses is the gcc thing*/

#include "list-header.h"
#include "mail-header.h"



/*'internal' functions I guess*/
int   inHeaders (int x, int y);
void  incX      (List* lines, int* x, int* y);
void  incY      (List* lines, int* x, int* y);
void  decX      (List* lines, int* x, int* y);
void  decY      (List* lines, int* x, int* y);
void  backspace (List* lines, int* x, int* y);
void  enter     (List* lines, int* x, int* y, int mode);

void  renderHeaderMenu (List* lines, int header_x, int header_y, Mail* mail);


#define HEADER_MENU_X 2
#define HEADER_MENU_Y 10


int inHeaders(int x, int y) {
    if(y < HEADER_NUM) return 1;
    return 0;
}


List* getLine(List* list, int line_num) {
    return *(List**)getListItem(list, line_num)->data;
}

void addLine (List* lines, char data[], int pos) {
    List* new_line = createListFromArray(sizeof(char), data, strlen(data));
    insert(lines, &new_line, pos);
    return;
}


void incX(List* lines, int* x, int* y){
    if (*x < listSize(getLine(lines, *y)) ) *x += 1;
    else if(*y < listSize(lines)-1) {
        if(*y == 3) {
            *x = 0;
            *y = HEADER_NUM-1;
        }
        else if(inHeaders(*x, *y)) *x = HEADER_WIDTH-1;
        else                       *x = 0;
        *y += 1;
    }
    return;
}

void incY(List* lines, int* x, int* y){
    if(*y >= listSize(lines)-1) {
        *x = listSize(getLine(lines, *y));
        return;
    }
    if(*y == 3) {
        *x = listSize(getLine(lines, 6));
        *y = 6;
        return;
    }
    if(inHeaders(*x, *y)) {
        *x = HEADER_WIDTH-1;
    } 
    else if (listSize(getLine(lines, *y + 1)) < *x) {
        *x = listSize(getLine(lines, *y + 1));
    }
    *y += 1;
}

void decX(List* lines, int* x, int* y){
    if(inHeaders(*x, *y)) {
        if(*x == HEADER_WIDTH-1) {
            if(*y == 0) {
                *x = HEADER_WIDTH-1;
                return;
            }
            *y -= 1;
            *x = listSize(getLine(lines, *y));
            return;
        }


        *x -= 1;
        return;
    }


    if(*x > 0) {
        *x -= 1;
        return;
    }


    if(*y == HEADER_NUM) *y -= 2;
    *y -= 1;
    *x = listSize(getLine(lines, *y));
    return;
}

void decY(List* lines, int* x, int* y){
    if(*y == 0) return;
    if(*y == HEADER_NUM) {
        *x = listSize(getLine(lines, *y - 1));
        *y -= 3;
        return;
    }
    if(listSize(getLine(lines, *y - 1)) < *x) {
        *x = listSize(getLine(lines, *y - 1));
    }
    *y -= 1;
    return;
}

void backspace(List* lines, int* x, int* y){
    if(inHeaders(*x, *y)) {
        if(*x == HEADER_WIDTH-1)
            return;

        *x -= 1;
        removeListItem(getLine(lines, *y), *x);
        renderFrame(lines, *x, *y); /*for some reason the character stays visible under the cursor otherwise*/
        return;
    }


    if(*x == 0) {
        if(*y == HEADER_NUM) 
            return;

        *y -= 1;
        *x = listSize(getLine(lines, *y));

        ListItem* firstLine = (ListItem*) getListItem(lines, *y);
        ListItem* secondLine = firstLine->successor;
        ListItem* thirdLine = firstLine->successor->successor;

        extendByList(*(List**) firstLine->data, *(List**) secondLine->data, 1);
        free(firstLine->successor->data);
        firstLine->successor = thirdLine;

        renderFrame(lines, *x, *y);
        return;
    }


    *x -= 1;
    removeListItem(getLine(lines, *y), *x);
    renderFrame(lines, *x, *y); /*for some reason the character stays visible under the cursor otherwise*/
    return;

    return;
}

void enter(List* lines, int* x, int* y, int mode){
    *y += 1;
    if (*y == 4) {
        *x = 0;
        *y += 2;
    } 
    else if (inHeaders(*x, *y)) {
        *x = HEADER_WIDTH-1;
    }
    else {
        *x = 0;
        if(mode == 1) addLine(lines, "", *y);
    }

    if(mode == 0)
        if(*y > listSize(lines))
            *y -= 1;

    renderFrame(lines, *x, *y);
}




void renderLine (List* lines, int line_num) {
    clrtoeol();
    mvprintw(line_num, 0, "%s", readList(getLine(lines, line_num), "str"));
}

void renderFrame (List* lines, int x, int y) {
    for(int i=0; i<listSize(lines); i++)
        renderLine(lines, i);
    move(listSize(lines), 0);
    clrtoeol();
    move(y, x);
}

void renderHeaderMenu (List* lines, int header_x, int header_y, Mail* mail) {
    for(int i=0; i<4; i++)
        renderLine(lines, i);


    mvprintw(HEADER_MENU_Y-4, HEADER_MENU_X, "    R S");
    mvprintw(HEADER_MENU_Y-3, HEADER_MENU_X, "    E E");
    mvprintw(HEADER_MENU_Y-2, HEADER_MENU_X, "  C C N");
    mvprintw(HEADER_MENU_Y-1, HEADER_MENU_X, "- C V D");
    for(int i=0; i<USERS_NUM; i++) {
        mvprintw(HEADER_MENU_Y + i, HEADER_MENU_X, "         %s", names[i]);
        int relation = getRelationToMail(mail, i);
        mvprintw(HEADER_MENU_Y + i, HEADER_MENU_X + relation*2, "o");
    }
    move(HEADER_MENU_Y + header_y, HEADER_MENU_X + header_x*2);
}




void renderMail(Mail *mail, int mode){
    /*mode:        0 ~ read only   1 ~ read and write*/
    /*header_mode: 0 ~ edit text   1 ~ edit id*/
    int header_mode = 0;


    if(mail == NULL) {
        printf("Error at renderMail(): provided mail is NULL");
        return;
    }
    if(mail->lines == NULL) {
        printf("Error at renderMail(): provided mail is empty");
    }

    /* ------------------------------ initialisation ------------------------------ */
    List* lines = mail->lines;
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    int cursor_y = 0, cursor_x = HEADER_WIDTH-1;
    int header_y = 0, header_x = 0;
    renderFrame(lines, cursor_x, cursor_y);


    /* ------------------------------ main loop ------------------------------ */

    int ch;
    while ((ch = getch()) != 27) { /*this is the escape key*/
        switch (ch) {
            case KEY_LEFT:
                if(!header_mode)      decX(lines, &cursor_x, &cursor_y);
                else if(header_x > 0) header_x -= 1;
                break;
            case KEY_RIGHT:
                if(!header_mode)      incX(lines, &cursor_x, &cursor_y);
                else if(header_x < 3) header_x += 1;
                break;
            case KEY_UP:
                if(!header_mode)      decY(lines, &cursor_x, &cursor_y);
                else if(header_y > 0) header_y --;
                break;
            case KEY_DOWN:
                if(!header_mode)      incY(lines, &cursor_x, &cursor_y);
                else if(header_y < USERS_NUM-1)   header_y ++;
                break;
            case KEY_BACKSPACE:
                if(!header_mode) {
                    if(mode == 0)     decX(lines, &cursor_x, &cursor_y);
                    if(mode == 1)     backspace(lines, &cursor_x, &cursor_y);
                }
                break;
            case '\n': /*ENTER*/
                if (!header_mode)     enter(lines, &cursor_x, &cursor_y, mode);
                else {
                    if(mode == 1) setRelationToMail(mail, header_y, header_x);
                    if(header_y == USERS_NUM-1) {
                        header_mode = !header_mode;
                        header_x = getRelationToMail(mail, 0);
                        header_y = 0;
                        clear();
                    } else {
                        header_y ++;
                        header_x = getRelationToMail(mail, header_y);
                    }
                }
                break;
            case '\t': /*TAB*/
                header_mode = !header_mode;
                header_x = getRelationToMail(mail, 0);
                header_y = 0;
                clear();
                break;
            default:
                if(!header_mode) {
                    if(mode == 1) insert(getLine(lines, cursor_y), &ch, cursor_x);
                    incX(lines, &cursor_x, &cursor_y);
                }
                break;

        }
        if(header_mode) renderHeaderMenu(lines, header_x, header_y, mail);
        else            renderFrame(lines, cursor_x, cursor_y);//renderLine(cursor_y);
        refresh();
    }

    /* ------------------------------ finishing the process ------------------------------ */
    endwin();


    return;
}



