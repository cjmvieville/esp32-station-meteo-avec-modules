#include "sm_board.h"

board  * boards;
const char separator ='|'; //séparateur utilisé pour distinguer les types de mesure de chaque valeur
int nbBoards = 0;
// La page html 
String index_html = "";
String board_html = "";
String boards_html = "";

int _getNbBoards(){
  return nbBoards;
}
String _getBoard_html(){
  return board_html;
}
String _getBoards_html(){
  return boards_html;
}

bool setup_Board(){ 
  index_html = readHtml_File("/index.html");
  board_html= readHtml_File("/board.html");
  char * path = "/boards.csv";
  File file = SD_MMC.open(path);
  // détermine le nombre de lignes pour allouer conveneablement la mémoire
  int nb = 0;
  nbBoards = 0;
  while(file.available()){
    nb++;
    file.readStringUntil('\n');
  }
  file.close();
  file = SD_MMC.open(path);
  boards = (board *) calloc(nb, sizeof(board));
  const char * sep=";";
  while(file.available()){
    for(int i = 0; i< nb ; i++){
      char * _record = (char *) malloc(100);
      int l= file.readBytesUntil('\n', _record,99);
      _record[l] = 0;
      char * token = strtok(_record, sep);
      if(token!=NULL) {
        boards[i].boardId = atoi(token);
        
        token = strtok(NULL, sep);    
        if(token!=NULL) {
          boards[i].owner = (char *) malloc(strlen(token)+1);
          memcpy(boards[i].owner, token, strlen(token)+1 );
          token = strtok(NULL, sep);
          if(token!=NULL) {
            boards[i].mesureTypes = (char *) malloc(strlen(token)+1);
            memcpy(boards[i].mesureTypes, token, strlen(token)+1);
            token = strtok(NULL, sep);
            if(token!=NULL) {
              boards[i].unites = (char *) malloc(strlen(token)+1);
              memcpy(boards[i].unites, token, strlen(token)+1);
              token = strtok(NULL, sep);
              if(token!=NULL) {
                boards[i].titles = (char *) malloc(strlen(token)+1);
                memcpy(boards[i].titles, token, strlen(token)+1);
              }
            }
            boards[i].payloads = (String *) calloc(getNumberOfValues(boards[i].mesureTypes), sizeof(String));
            //remplir board_html pour chaque mesure de la board
            for(int j=0; j<getNumberOfValues(boards[i].mesureTypes); j++){
              board_html= readHtml_File("/board.html");
              board_html.replace("%id%",String(boards[i].boardId));
              board_html.replace("%owner%",String(boards[i].owner));
              board_html.replace("%typemesure%",getBoardString(boards[i].mesureTypes,j));
              board_html.replace("%unite%",getBoardString(boards[i].unites,j));
              board_html.replace("%title%",getBoardString(boards[i].titles,j));
              boards_html+=board_html;
              boards[i].payloads[j]="";
            }
            nbBoards++;
          }
        }
      }
      free(_record);
    }
    //remplacer %boards% dans undex_html par boards_html
    index_html.replace("%boards",boards_html);
  }
  file.close();
  // Affiche les cartes préparées
  // sera utile quand on les initialisera depuis les données de Firebase RTDB
  for(int i=0; i<nb;i++){
    Serial.print(boardToString(boards[i]));
    Serial.println("***********************************");
  }
  Serial.println("fin initBoards");
  return true;
}
String _getValue(String data, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
String _getBoardValue(board *_board, int index) {
  return _getValue(_board->mesureTypes, index);
}
// retourne  d'une mesure pour une carte et pour sa ième valeur (index)
char getBoardValueAsChar(board *_board, int index) {
  String data=_getBoardValue(_board, index);
  return data.charAt(0);
}
// Retourne la carte à partir de son identifiant
board * getBoardById(int boardId){
  for(int i=0; i<getMaxBoards();i++){
    if(boards[i].boardId==boardId){
      return &boards[i];
    }
  }
  return NULL;
}
board * getBoardByIndex(int index){
	return &boards[index];
}


String getIndex_html(){
	return index_html;
}

int getMaxBoards(){
  return nbBoards;
}
String getBoardString(String myString, int index){
  String sep=String(separator);
  int j=0;
  String res="";
  for(int i = 0; i<index+1; i++){
    int k=myString.indexOf(sep,j);
    res=k>=0?myString.substring(j,k):myString.substring(j);
    j=k+1;
  }
  return res;
}
// retourne une string décrivant les données d'une carte
String boardToString (board _board){
  String _string = "boardId : " + String(_board.boardId) + "\n";
  _string += "owner : " + String(_board.owner) + "\n"; 
  Serial.println(String(_board.owner));
  Serial.println(String(_board.mesureTypes));
  Serial.println(String(_board.unites));
  Serial.println(String(_board.titles));
  for(int i=0; i<getNumberOfValues(_board.mesureTypes); i++){
      _string += "type de mesure : " + getBoardString(_board.mesureTypes,i) + " ; " + getBoardString(_board.unites,i) + " ; " + getBoardString(_board.titles,i) + " ; " + "\n";
      _string += "payload =" +_board.payloads[i]+ "\n";
  }
  Serial.println(_string);
  return _string; 
}
int getNumberOfValues( String data){
  int _count_sep = 1;
  for(int i=0; i<data.length();i++) {
      if(data[i] == separator){
        _count_sep++;
      } 
  }
  return _count_sep;
}
