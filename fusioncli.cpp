#include "fusioncli.h"


FusionCLI::FusionCLI(QObject *parent) : QObject(parent)
{
    if(!db.init())
        return;

    q = new QTextStream(stdout);
    refreshList();
}


void FusionCLI::execute(int argc, char *argv[]) {

    if(argc>1)
   {
       QString arg = argv[1];
       if(arg=="-g"||arg=="--allGames")
           getAllGames();
       else if(arg=="-r"||arg=="--refresh")
           refreshList();
       else if(arg=="-l"||arg=="--launch")
           launchByID(QString(argv[2]));
   }
}

void FusionCLI::launchByID(QString ID) {
    bool noError  = true;
    int gameID  = ID.toInt(&noError, 10);

    if(!noError)
    {
        *q << "Cannot read ID, which is: " << ID << endl;
        return;
    }

    FGame *game = db.getGame(gameID);
    if(game == NULL)
    {
        *q << "Can't find game with ID: " << ID << endl;
        return;
    }

    qDebug() << "launch game: " << game->getName();
    game->execute();
}


void FusionCLI::refreshList() {
    gameList.clear();
    gameList = db.getGameList();
}

void FusionCLI::getAllGames() {
    qDebug() << "print all games.";
    QList<QVariant> games;
    qDebug() << "Found " << gameList.length() << " games.";

    for(int i=0;i<gameList.length();++i) {
       games.append(getGame(&gameList[i]));
    }


    print(games, "Games");
}

QMap<QString, QVariant> FusionCLI::getGame(FGame *game){
    qDebug() << "Print Game: "<< game->getName();

   QMap<QString, QVariant> map;
   map.insert("name", game->getName().replace("\\", "\\\\"));
   map.insert("id", game->dbId);
   map.insert("exe", game->getExe().replace("\\", "\\\\"));
   map.insert("path", game->getPath().replace("\\", "\\\\"));
   map.insert("type", game->getType());
   map.insert("banner", game->getBanner());
   map.insert("poster", game->getBoxart());
   map.insert("clearart", game->getClearart());
   map.insert("clearlogo", game->getClearart());

   return map;
}



void FusionCLI::print(QList<QVariant> list, QString title) {
    qDebug() << "Print QList<QVariant>";

    *q << "{ \"" << title << "\" : [" << endl;


    int i;
    for(i=0;i<list.length()-1;++i) {
        print(list[i]);
        *q << ", " << endl;
    }

    print(list[i]);


   *q << "] }" << endl;
}


void FusionCLI::print(QVariant list) {
    QString type = list.typeName();
    if(type == "QVariantMap") {
        print(list.toMap());

    } else {
        qDebug() << "Type: " << type;

    }
}


void FusionCLI::print(QMap<QString, QVariant> list) {
    *q << "{" << endl;
    QList<QString> keys = list.keys();
    QList<QVariant> values = list.values();


    int i;
    for(i=0;i<list.size()-1;++i)
        *q << "\"" <<  keys[i]<< "\": \""  << values[i].toString() << "\", " << endl;

    *q << "\"" <<  keys[i]<< "\": \""  << values[i].toString() << "\" " << endl;


    *q << "} ";
}

