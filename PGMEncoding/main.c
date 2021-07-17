//
//  main.swift
//  Image compression with Run Length Encoding (RLE) method
//
//  Created by Yusuf Turan on 17.07.2021.
//  Developed by Mehmet Turan at 14.06.2021.
//



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  // File reading buffer
#define SIZE 512000

typedef struct info{
  char type[8];
  char comment[32];
  int column;
  int row;
  int maxGray;
}Info;

typedef struct pgm{
  Info *inf;
  int **matrix;
}PGM;
PGM *pgmDetail = NULL;

typedef struct item{
  int count;
  int color;
  struct item *next;
}Item;

typedef struct rle{
  Info *inf;
  Item* itm;
}RLE;
RLE *rleDetail = NULL;

  // PGM Operations
void loadPgmFile(char* fileName);
void printPgmFileInfo(void);
void compress(void);
void printRleFile(void);

  // RLE Operations
void loadRleFile(char* fileName);
void decompress(void);
int  checkRleIntegrity(int printResults);
void changeColor(int currentColor, int targetColor);
void changeColorByCoord(int row, int column, int color);
void calculateHistogram(void);
void checkAndFixErrors(void);
void printNewEncoded(void);

  // Other Operations
void printOperations(int operation);
void printDashes(void);
void clearScreen(void);

int **matrixAlloc(int row, int col);
void matrixFree(int **matrix, int row);

int main(){
  char fileName[64];
  
  printOperations(1);
  
  int op1, op2;
  int sub;
  int curColor, tarColor, row, column;
  
  while(1){
    
    printf("\n Select an operation to proceed: ");
    
    if(scanf("%d", &op1)==1) {
      switch(op1){
        case 1:
          clearScreen();
          sub=1;
          printOperations(2);
          while(sub){
            printf("\n Select an operation to proceed: ");
            if(scanf("%d", &op2)==1){
              switch(op2){
                case 1:
                  printf("\n Please enter a PGM file name:");
                  scanf("%s", fileName);
                  printDashes();
                  loadPgmFile(fileName);
                  printDashes();
                  break;
                case 2:
                  printDashes();
                  compress();
                  printDashes();
                  break;
                case 3:
                  printDashes();
                  printPgmFileInfo();
                  printDashes();
                  break;
                case 4:
                  printDashes();
                  printRleFile();
                  printDashes();
                  break;
                case 5:
                  clearScreen();
                  printOperations(2);
                  break;
                case 6:
                  clearScreen();
                  sub=0;
                  printOperations(1);
                  break;
                default:
                  printDashes();
                  printf("\n Invalid process request!");
                  printDashes();
                  break;
              }
              if(sub==0)
                break;
            }
            else{
              printDashes();
              printf("\n An integer value expected. Please restart!");
              printDashes();
              printf("\n\n");
              exit(EXIT_FAILURE);
            }
          }
          break;
        case 2:
          clearScreen();
          sub=1;
          printOperations(3);
          while(sub){
            printf("\n Select an operation to proceed: ");
            if(scanf("%d", &op2)==1){
              switch(op2){
                case 1:
                  printf("\n Please enter a RLE file name:");
                  scanf("%s", fileName);
                  printDashes();
                  loadRleFile(fileName);
                  printDashes();
                  break;
                case 2:
                  printDashes();
                  decompress();
                  printDashes();
                  break;
                case 3:
                  printDashes();
                  checkRleIntegrity(1);
                  printDashes();
                  break;
                case 4:
                  printf("\n Enter a color and new color number(Ex:5 10): ");
                  scanf("%d%d", &curColor, &tarColor);
                  printDashes();
                  if(curColor > 255 || tarColor > 255)
                    printf("\n Please enter a valid color from 0 to 255!");
                  else
                    changeColor(curColor, tarColor);
                  printDashes();
                  break;
                case 5:
                  printf("\n Enter row, colum and color(Ex:5 2 3): ");
                  scanf("%d %d %d", &row, &column, &tarColor);
                  printDashes();
                  if( row<0 || column<0 || tarColor > 255)
                    printf("\n Enter valid values(Row and column >= 0, color <256)!");
                  else{
                    changeColorByCoord(row, column, tarColor);
                  }
                  printDashes();
                  break;
                case 6:
                  printDashes();
                  calculateHistogram();
                  printDashes();
                  break;
                case 7:
                  clearScreen();
                  printOperations(3);
                  break;
                case 8:
                  clearScreen();
                  sub=0;
                  printOperations(1);
                  break;
                default:
                  printDashes();
                  printf("\n Invalid process request!");
                  printDashes();
                  break;
              }
              if(sub==0)
                break;
            }
            else{
              printDashes();
              printf("\n An integer value expected. Please restart!");
              printDashes();
              printf("\n\n");
              exit(EXIT_FAILURE);
            }
          }
          break;
        case 3:
          clearScreen();
          printOperations(1);
          break;
        case 4:
          exit(EXIT_SUCCESS);
          break;
        default:
          printDashes();
          printf("\n Invalid process request!");
          printDashes();
          break;
      }
    }
    else{
      printDashes();
      printf("\n An integer value expected. Please restart!");
      printDashes();
      printf("\n\n");
      exit(EXIT_FAILURE);
    }
  }
  return 0;
}

  ////////////////////////////////
  ///   PGM Operations Begin    //
  ////////////////////////////////

  // Loads PGM file contents to 'pgmDetail' variable
  // It is a requirement to be able to run PGM file operations.
void loadPgmFile(char fileName[64]){
  
    // if pgmDetail not null free allocated matrix
  if(pgmDetail){
    matrixFree(pgmDetail->matrix, pgmDetail->inf->row);
    free(pgmDetail->inf);
  }
  
  FILE* fp;
  char line[SIZE];
  
  int lineIndex;
  fp = fopen(fileName, "r");
  
  if (fp == NULL){
    printf("\n File can't be opened, please check file name!");
  }
  else{
    int **matrix;
    Info *inf;
    
    char * token;
    int i, j, k;
    
    
    
    lineIndex=0;
    i=0;
    j=0;
    inf = (Info*)malloc(sizeof(Info));
    
    while (fgets(line, SIZE, fp) != NULL){
      
        // remove line break
      size_t len = strlen(line);
      if (len > 0 && line[len-1] == '\n')
        line[--len] = '\0';
      
      switch(lineIndex){
        case 0:
          if(strcmp(line,"P2")==1){
            printf("\n Please load a valid P2 type pgm file!");
          }
          else
            strcpy(inf->type, line);
          break;
        case 1:
          strcpy(inf->comment, line);
          if(inf->comment[0] !='#') {
            strcpy(inf->comment, "# no comment in loaded file!");
            lineIndex++;
            goto NEXT;
          }
          
          break;
        case 2:
        NEXT:
          token = strtok(line, " ");
          k=0;
          
          while( token != NULL ) {
            if(k == 0)
              inf->column = atoi(token);
            else if(k == 1){
              inf->row = atoi(token);
              matrix = matrixAlloc(inf->row, inf->column);
            }
            
            token = strtok(NULL, " ");
            k++;
          }
          break;
          
        case 3:
          inf->maxGray=atoi(line);
          break;
          
        default:
          free(token);
          
          token = strtok(line, " ");
          while( i < inf->row && token != NULL ) {
            matrix[i][j]=atoi(token);
            
            if(j+1 < inf->column){
              j++;
            }
            else{
              j=0;
              i++;
            }
            
            token = strtok(NULL, " ");
          }
          
          break;
      }
      lineIndex++;
    }
    pgmDetail=(PGM*)malloc(sizeof(PGM));
    pgmDetail->inf=inf;
    
    pgmDetail->matrix=matrix;
    
    fclose(fp);
    
    if(strcmp(inf->type, "P2")==0)
      printf("\n PGM file loaded successfully.");
    else
      printf("\n PGM file could not be loaded.");
  }
}

  // Shows PGM file definition on console
void printPgmFileInfo(){
  if(pgmDetail){
    Info *inf=pgmDetail->inf;
    printf("\n Type: %s \n Comment: %s \n Columns: %d \n Rows: %d \n Max Gray: %d", inf->type, inf->comment, inf->column, inf->row, inf->maxGray);
  }
  else
    printf("\n You must load a PGM file to display file information!");
}

  // Encodes PGM to RLE format
void compress(){
  if(pgmDetail){
    int **matrix=pgmDetail->matrix;
    Info *inf= pgmDetail->inf;
    int i, j, k;
    int lastCol, currentCol, count, color;
    count=0;
    FILE * fp =fopen("test_encoded.txt","w");
    
    fprintf(fp,"%d %d ", inf->row, inf->column);
    
    lastCol=-1;
    k=0;
    for( i=0; i<inf->row;i++){
      for(j=0; j< inf->column;j++){
        currentCol=matrix[i][j];
        
        if(lastCol>-1 && lastCol != currentCol){
          fprintf(fp,"%d %d ",count, lastCol);
          count=0;
        }
        
        count++;
        lastCol = currentCol;
        
      }
    }
    fprintf(fp,"%d %d",count, lastCol);
    fclose(fp);
    printf("\n RLE file created succefully to 'test_encoded.txt'");
  }
  else
    printf("\n You must load a PGM file to compress!");
}

  // Shows RLE file content on console
void printRleFile(){
  if(pgmDetail){
    int lastCol, currentCol, count;
    int i, j, k;
    int **matrix=pgmDetail->matrix;
    Info *inf= pgmDetail->inf;
    printf("\nRow Col\n");
    printf("\%d   %d \n", inf->row, inf->column);
    
    lastCol=-1;
    count=0;
    k=0;
    for( i=0; i<inf->row;i++){
      for(j=0; j< inf->column;j++){
        currentCol=matrix[i][j];
        
        if(lastCol>-1 && lastCol != currentCol){
          printf("%d %d ",count, lastCol);
          count=0;
        }
        
        count++;
        lastCol = currentCol;
        
      }
    }
    printf("%d %d",count, lastCol);
    
  }else{
    printf("\n You must load a PGM file to display file information!");
  }
  
  
}

  ////////////////////////////////
  ///   PGM Operations End      //
  ////////////////////////////////


  ////////////////////////////////
  ///   RLE Operations Begin    //
  ////////////////////////////////

  // Loads RLE file contents to rleDetail variable
  // It is a requirement to be able to run RLE file operations.
void loadRleFile(char fileName[64]){
  FILE* fp;
  char line[SIZE];
  int lineIndex;
  
  fp = fopen(fileName, "r");
  
  if (fp == NULL){
    printf("\n File can't be opened. Please check file name!");
  }
  else{
    Info *inf;
    
    Item *headItem;
    Item *curItem;
    
    char *token;
    int j, k, count;
    
    inf = (Info*)malloc(sizeof(Info));
    
    headItem = NULL;
    curItem = NULL;
    char *comment="# created by RLE to PGM decoder";
    strcpy(inf->type, "P2");
    strcpy(inf->comment, comment);
    inf->maxGray=255;
    
    while (fgets(line, SIZE, fp) != NULL){
      
        // remove line break
      size_t len = strlen(line);
      if (len > 0 && line[len-1] == '\n')
        line[--len] = '\0';
      
      token = strtok(line, " ");
      
      
      j = 0;
      while( token != NULL ) {
        if(j==0){
          inf->row=atoi(token);
        }
        else if(j==1){
          inf->column=atoi(token);
        }
        else{
          
          if(j % 2 == 0){
            count = atoi(token);
          }
          else{
            
            Item *newItem=(Item*)malloc(sizeof(Item));
            newItem->color = atoi(token);
            newItem->count = count;
            newItem->next=NULL;
            if(headItem == NULL){
              headItem = newItem;
              curItem = headItem;
            } else {
              curItem->next = newItem;
              curItem = curItem->next;
            }
          }
          
        }
        
        token = strtok(NULL, " ");
        
        j++;
      }
    }
    rleDetail = (RLE*)malloc(sizeof(RLE));
    rleDetail->itm = headItem;
    rleDetail-> inf = inf;
    
    fclose(fp);
    
    if(checkRleIntegrity(0)== 0){
      printf("\n RLE file loaded successfully.");
    }else{
      free(rleDetail->inf);
      free(rleDetail->itm);
      free(rleDetail);
    }
  }
  
  
}

  // Checks loaded RLE file according to conditions in project
int checkRleIntegrity(int printResults){
  int error[3]={0, 0, 0};
  int errCount=0;
  if(rleDetail) {
    
    int pixel;
    Item *tmpItem = rleDetail->itm;
    Item *prevItem = NULL;
    
    Info *inf = rleDetail->inf;
    pixel=0;
    while(tmpItem){
        // check number same color are sequential
      if(prevItem != NULL && prevItem->color == tmpItem->color){
        error[2] = 1;
      }
      if(tmpItem->color>255){
        error[1] = 1;
      }
      pixel += tmpItem->count;
      
      prevItem = tmpItem;
      tmpItem = tmpItem->next;
      
    }
    if(pixel != (inf->column)*(inf->row))
      error[0] = 1;
    
    errCount= error[0] + error[1] + error[2];
    
    if(errCount > 0){
      printf("\n There are some errors in loaded RLE file");
    }
    if(errCount > 0 || printResults > 0){
      printf("\n Number of pixel validation      :  %s", (error[0]>0 ? "Bad":"Good"));
      printf("\n Color range is in between 0-255 :  %s", (error[1]>0 ? "Bad":"Good"));
      printf("\n Sequential color existence      :  %s", (error[2]>0 ? "Bad":"Good"));
    }
  }else{
    printf("\n You must load a RLE file to check integrity!");
  }
  return errCount;
}

  // Decodes loaded RLE file to PGM format
  // A RLE file must be loaded before this operation
void decompress(){
  if(rleDetail) {
    if(checkRleIntegrity(1) == 0){
      Item *tmpItem = rleDetail->itm;
      Info *inf = rleDetail->inf;
      
      int i, j, k;
      
      FILE * fp = fopen("test_decoded.pgm", "w");
      fprintf(fp,"%s\n",inf->type);
      fprintf(fp,"%s \n",inf->comment);
      fprintf(fp,"%d %d\n", inf->column , inf->row);
      fprintf(fp,"%d\n", inf->maxGray);
      
      i = 0;
      k = 1;
      int color, count;
      
      while(tmpItem){
        
        color=tmpItem->color;
        count=tmpItem->count;
        
        j = 0;
        
        while(j < count) {
          fprintf(fp,"%d", color);
          
          if(k % inf->column == 0){
            fprintf(fp,"\n");
          }
          else{
            fprintf(fp," ");
          }
          
          j++;
          k++;
        }
        
        tmpItem = tmpItem->next;
      }
      
      fclose(fp);
      printf("\n PGM file created succefully to 'test_decoded.pgm'");
    }
    else{
      printf("\n Decoding failed due to the errors in loaded RLE file.");
    }
  }
  else{
    printf("\n You must load a RLE file to decompress/decode!");
  }
}

  // Changes a color to new one
  // A RLE file must be loaded before this operation
void changeColor(int currentColor, int targetColor ){
  if(rleDetail){
    Item *tmp=rleDetail->itm;
    while(tmp){
      if(tmp->color == currentColor){
        tmp->color = targetColor;
      }
      tmp=tmp->next;
    }
    printf("\n Changing color from '%d' to '%d' completed successfully.", currentColor, targetColor);
    checkAndFixErrors();
    printNewEncoded();
      //decompress("test_decoded.txt");
  }
  else{
    printf("\n You must load a RLE file to change color!");
  }
}

  // Create new RLE item to insert new color according to given coordinates
Item* createNewItem(int color, int count){
  Item *newItem =(Item*)malloc(sizeof(Item));
  newItem->color=color;
  newItem->count=count;
  newItem->next=NULL;
  return newItem;
}

  // Change color at given row and columns according to project requirements
  // This task achieved without opening complete file.
  // Only given coordinates are taken into account.
void changeColorByCoord(int row, int column, int color){
  if(rleDetail){
    Info *inf=rleDetail->inf;
    Item *tmpItem=rleDetail->itm;
    Item *tmp=NULL;
    int position, counter, difference;
    
    position = row * inf->column + column + 1;
    counter=0;
    
    while(tmpItem) {
      counter += tmpItem->count;
      
        // if given coordinate is at the end or first item
      if(counter == position) {
        if(tmpItem-> count > 1){
          Item *newItem=createNewItem(color, 1);
          tmpItem->count = tmpItem-> count - 1;
          newItem->next = tmpItem->next;
          tmpItem->next = newItem;
        } else{
          tmpItem->color=color;
        }
        break;
      }
        //if given coordinate is between sequential occurrence
      else if(counter > position){
        difference = counter - position;
        tmpItem->count -= (difference+1);
        
        Item *newNextItem = createNewItem(tmpItem->color, difference);
        Item *newItem = createNewItem(color, 1);
        newItem->next = newNextItem;
        newNextItem->next = tmpItem->next;
        tmpItem->next = newItem;
        
        break;
      }
      
      tmpItem = tmpItem->next;
    }
    printf("\n Changing color of coord[%d, %d] to '%d' completed successfully.", row, column, color);
    
    checkAndFixErrors();
    printNewEncoded();
  }
  else{
    printf("\n You must load a RLE file to change color!");
  }
}

  // Initialize new color histogram to count
Item *headHist=NULL;
void addHist(int count, int color){
  Item *tmpHist = headHist;
  Item *lastHist = NULL;
  int added;
  
  Item *newHist = (Item*)malloc(sizeof(Item));
  newHist->color=color;
  newHist->count=count;
  newHist->next=NULL;
  
  if(headHist == NULL){
    headHist=newHist;
  }
  else{
    added=0;
    while(tmpHist){
      if(tmpHist->color == color){
        tmpHist->count += count;
        added = 1;
      }
      lastHist = tmpHist;
      tmpHist = tmpHist->next;
    }
    if(added==0 && lastHist!=NULL){
      lastHist->next=newHist;
    }
  }
}

  // Prints color histogram to file and console
void printHistogram(){
  
  Item *tmpHist=headHist;
  FILE * fp =fopen("test_encoded.txt","w");
  fprintf(fp,"Color\t\t Count\n");
  printf("\n Color\t\t Count\n");
  while(tmpHist){
    fprintf(fp," %d\t\t  %d\n",tmpHist->color,tmpHist->count);
    printf(" %d\t\t  %d\n",tmpHist->color,tmpHist->count);
    tmpHist=tmpHist->next;
  }
  fclose(fp);
  printf("\n RLE histogram created succefully to 'test_encoded.txt'");
}

  // Calculates occurrence of each colors in RLE file
void calculateHistogram(){
  if(rleDetail){
    Item *histItem;
    Item *tmpItem=rleDetail->itm;
    while(tmpItem){
      addHist(tmpItem->count, tmpItem->color);
      tmpItem=tmpItem->next;
    }
    printHistogram();
  }else{
    printf("\n You must load a RLE file to calculate histogram!");
  }
}

  // Fix sequential occurrence error after color change
  // by merging sequential existence of new color
void checkAndFixErrors(){
  if(rleDetail){
    Item *tmpItem=rleDetail->itm;
    Item *lastItem=NULL;
    Item *tmp=NULL;
    
    while(tmpItem){
      if(lastItem){
        if(tmpItem->color == lastItem->color){
          tmp = tmpItem;
          lastItem->count += tmpItem->count;
          lastItem->next=tmpItem->next;
          free(tmp);
          tmpItem=lastItem;
        }
      }
      
      lastItem = tmpItem;
      tmpItem = tmpItem->next;
    }
  }
  
  
}

  // Prints new RLE file after change to test_encoded.txt
  // A RLE file must be loaded before this operation
void printNewEncoded(){
  
  if(rleDetail){
    Item *tmpItm = rleDetail->itm;
    Info *inf = rleDetail->inf;
    
    FILE * fp =fopen("test_encoded.txt","w");
    
    fprintf(fp,"%d %d ", inf->row, inf->column);
    
    while(tmpItm){
      fprintf(fp,"%d %d ",tmpItm->count, tmpItm->color);
      tmpItm =  tmpItm->next;
    }
    fclose(fp);
    printf("\n Results are succefully written to 'test_encoded.txt'");
  }else{
    printf("\n You must load a RLE file to continue!");
  }
  
}
  ////////////////////////////////
  ///   RLE Operations End      //
  ////////////////////////////////


  ////////////////////////////////
  ///  Other Operations Begin   //
  ////////////////////////////////

  // Prints operations to the console
void printOperations(int operation){
  
  if(operation == 1)
    printf("\n              MAIN MENU");
  else if(operation == 2)
    printf("\n              PGM file operations");
  else if(operation == 3)
    printf("\n              RLE file operations");
  
  printf("\n *******************************************************");
  if(operation == 1){
    printf("\n     1) PGM file operations");
    printf("\n     2) RLE file operations");
    printf("\n     3) Clear screen");
    printf("\n     4) Exit");
  }
  else if(operation == 2){
    printf("\n     1) Load a valid PGM(P2) file");
    printf("\n     2) Convert PGM file to RLE file");
    printf("\n     3) Display PGM file info");
    printf("\n     4) Display RLE file content");
    printf("\n     5) Clear screen");
    printf("\n     6) <-- Back to main menu");
  }
  else if(operation == 3){
    printf("\n     1) Load a RLE file");
    printf("\n     2) Convert RLE file to PGM file");
    printf("\n     3) Control and print RLE file validty");
    printf("\n     4) Change a color value");
    printf("\n     5) Change color of given coordinate");
    printf("\n     6) Calculate picture histogram");
    printf("\n     7) Clear screen");
    printf("\n     8) <-- Back to main menu");
  }
  printf("\n *******************************************************");
  printf("\n");
}

  // Clear screen for ease of use
void clearScreen(){
  system("@cls||clear");
}

  // To add boundary for requested operation
void printDashes(){
  printf("\n -------------------------------------------------------");
}

  // To initalize a dynamic 2D matrix for PGM operation
int **matrixAlloc(int row, int col){
  int **result;
  int i;
  
  result = (int **)malloc(sizeof(int *) * row);
  if (result == NULL) {
    perror("An error occurred on memory allocation!");
    exit(EXIT_FAILURE);
  }
  
  for (i = 0; i < row; ++i) {
    result[i] = (int *)malloc(sizeof(int) * col);
    if (result[i] == NULL) {
      perror("An error occurred on memory allocation!");
      exit(EXIT_FAILURE);
    }
  }
  
  return result;
}

  // To free allocated matrix
void matrixFree(int **matrix, int row){
  int i;
  for (i = 0; i < row; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

  ////////////////////////////////
  ///   Other Operations End    //
  ////////////////////////////////

