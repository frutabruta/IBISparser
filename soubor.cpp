#include "soubor.h"
#include <QtDebug>
#include <QFile>
#include <QtXml>


Soubor::Soubor(QObject *parent) : QObject(parent)
{
    QString nazevSouboru="testhexdump.hex";
    cestaSouboruHex=QCoreApplication::applicationDirPath()+"/"+nazevSouboru;
}



QByteArray Soubor::otevriSoubor()
{
    QByteArray vysledek="";
    qDebug()<<"Soubor::otevriSoubor()";
    QDomDocument doc("mydocument");
    qDebug()<<"cesta k souboru je "<<cestaSouboruHex;
    QFile file(cestaSouboruHex);

    if (!file.open(QIODevice::ReadOnly))
    {
        //  emit odesliChybovouHlasku("soubor se nepovedlo otevrit");
        QString neotevruSoubor="soubor se nepovedlo otevrit";
        qDebug()<<neotevruSoubor;
        emit odesliChybovouHlasku(neotevruSoubor);
        return vysledek;
    }


    vysledek=file.readAll();
    file.close();

    emit    odesliChybovouHlasku("soubor otevren");
    qDebug()<<"uspech";
    QDomElement koren=doc.firstChildElement();

    //qDebug()<<vysledek;

    return vysledek;
    //this->vypis(vysledek);

    // najdiCestaHlaseni(doc);

}


void Soubor::vypis(QByteArray vstup)
{
    qDebug()<<"Soubor::vypis";
    qDebug()<<"delka velikost je "<<vstup.size()<<" delka je "<<vstup.length();
    int delka=vstup.length();
    QVector<QString> radky;
    QVector<QString> radkyHex;
    QString radek="";
    QString radekHex="";
    for (int i=0; i<(delka);i++)
    {
        radek+=zavorky(vstup.at(i));
        radekHex+=" ";
        radekHex+=this->formatHex(this->hexDoplnNulu(vstup.at(i)));
        if (vstup.at(i) ==0x0D)
        {
            i++;
            radek+=zavorky(vstup.at(i));
            radekHex+=" ";
            radekHex+=this->formatHex(this->hexDoplnNulu(vstup.at(i)));

            radky.append(radek);
            radkyHex.append(radekHex);
            radek="";
            radekHex="";
            qDebug()<<"konec prikazu";

        }

        qDebug()<<i<<" "<<vstup[i];
    }
    qDebug()<<"konec";

    QString htmlSoubor="";
    QString htmlRadky="";
    for (int g=0;g<radkyHex.length();g++)
    {
        htmlRadky+=tabulkaRadek(tabulkaBunka(radky.at(g))+tabulkaBunka(radkyHex.at(g)));
        qDebug()<<radky.at(g)<<" "<<radkyHex.at(g);
    }

    htmlSoubor=this->wrapper(htmlRadky);
    zapisHtml(htmlSoubor);
    qDebug()<<htmlSoubor;

}


void Soubor::zpracujRs485(QByteArray vstup)
{
    qDebug()<<"Soubor::zpracujRs485";
    int delka=vstup.length();
    qDebug()<<"delka velikost souboru je "<<delka;

    QVector<QString> radky;
    QVector<QString> radkyHex;
    QString radek="";
    QString radekHex="";

    int delkaDatRadku=0;
    int indexZnakuRadku=0;
    short adresa=0;
    /*
    for (short i=0;i<255 ;i++ )
    {
        char rrrr=i;
        QString retezec="";
        retezec=retezec+rrrr;
        qDebug()<<"i="<<i<<" retezec:"<<retezec<<" char:"<<rrrr<<" QString number HEX:"<<QString::number( vratCislo( rrrr),16);
    }
    */

    for (int i=0; i<(delka);i++)
    {

        switch(indexZnakuRadku)
        {
        case 0:
        {
            adresa=vratCislo( vstup.at(i));
            qDebug()<<"adresa panelu je "<<QString::number( adresa,16);


            radek+=zavorky(vstup.at(i));
            radekHex+=" ";
            radekHex+=this->formatHex(this->hexDoplnNulu(vstup.at(i)));
            indexZnakuRadku++;
            break;
        }
        case 1:
        {
            if(adresa==3)
            {
                delkaDatRadku=3;
            }
            else
            {
                delkaDatRadku=vratCislo( vstup.at(i));
            }

            qDebug()<<"delka zpravy je "<<QString::number(vstup.at(i))<<" "<<delkaDatRadku;
            radek+=zavorky(vstup.at(i));
            radekHex+=" ";
            radekHex+=this->formatHex(this->hexDoplnNulu(vstup.at(i)));


            indexZnakuRadku++;
            break;
        }
        default:
        {
            if(indexZnakuRadku==(delkaDatRadku+1))
            {
                radek+=zavorky(vstup.at(i));
                radekHex+=" ";
                radekHex+=this->formatHex(this->hexDoplnNulu(vstup.at(i)));
                radky.append(radek);
                radkyHex.append(radekHex);
                delkaDatRadku=0;
                indexZnakuRadku=0;

                qDebug()<<"konec prikazu, jsem na bytu "<<QString::number(vratCislo( vstup.at(i)),16)<<" "<<" XX"<<radekHex;
                radek="";
                radekHex="";
            }
            else
            {
                radek+=zavorky(vstup.at(i));
                radekHex+=" ";
                radekHex+=this->formatHex(this->hexDoplnNulu(vstup.at(i)));
                indexZnakuRadku++;
               // qDebug()<<"novy indexRadku "<<indexZnakuRadku<<" max:"<<delkaDatRadku;

            }

            break;
        }

        }



        //qDebug()<<i<<" "<<vstup[i];
    }
    qDebug()<<"konec";

    QString htmlSoubor="";
    QString htmlRadky="";
    for (int g=0;g<radkyHex.length();g++)
    {
        htmlRadky+=tabulkaRadek(tabulkaBunka(radky.at(g))+tabulkaBunka(radkyHex.at(g)));
        qDebug()<<radky.at(g)<<" "<<radkyHex.at(g);
    }

    htmlSoubor=this->wrapper(htmlRadky);
    zapisHtml(htmlSoubor);
    qDebug()<<htmlSoubor;

}


QString Soubor::zavorky(char vstup)
{
    QString vystup="";
    int vstupCislo=vratCislo(vstup);
    if (vstupCislo<0x20||vstupCislo>0x7F)
    {

        vystup+="<";
        vystup+=formatHex( hexDoplnNulu(vstup));
        vystup+=">";
    }
    else
    {
        vystup+=vstup;
    }
    return vystup;
}


QString Soubor::hexDoplnNulu(char vstup)
{


    // vystup=vstup&0x0000000011111111;
    short cislo=vratCislo(vstup);
    QString vystup=QString::number(cislo,16);

    if (cislo<0x10)
    {
        vystup="0"+vystup;
    }
    /*
    if(vystup.length()>2)
    {
        vystup=vystup.right(2);
    }*/
    return vystup;

}

QString Soubor::tabulkaBunka(QString vstup)
{
    QString vystup="<td>"+vstup+"</td>";
    return vystup;
}

QString Soubor::tabulkaRadek(QString vstup)
{
    QString vystup="<tr>"+vstup+"</tr>";
    return vystup;
}

QString Soubor::wrapper(QString vstup)
{
    QString vystup="<!DOCTYPE html><html>"+htmlHlavicka()+"<body><table>"+vstup+"<table><body></html>";
    return vystup;
}

void Soubor::zapisHtml(QString vstup)
{
    QFile file(cestaSouboruHtml);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // We're going to streaming text to the file
        QTextStream stream(&file);

        stream << vstup;
        file.close();
        QString zapsano="Writing finished";
        qDebug() << zapsano;
        emit odesliChybovouHlasku(zapsano);
    }
    else
    {
        QString chybovaHlaska="soubor nelze zapsat";
        qDebug()<<chybovaHlaska;
        emit odesliChybovouHlasku(chybovaHlaska);
    }


}

QString Soubor::htmlHlavicka()
{
    QString vystup="<head>"+this->htmlStyl()+"</head>";
    QString vystup2="<head><link rel=\"stylesheet\" href=\"mystyle.css\"></head>";
    return vystup;
}

QString Soubor::htmlStyl()
{
    QString styl="*{font-family: \"Courier New\", Courier, \"Lucida Sans Typewriter\", \"Lucida Typewriter\", monospace;color: black;}";
    styl+="td{border: 1px solid blue;}";
    styl+="span.escape{color:red;}";
    styl+="span.format{color:orange;}";
    styl+="span.lf{color:green;}";

    QString vystup="<style>"+styl+"</style>";
    return vystup;
}

QString Soubor::formatHex(QString vstup)
{
    QString vystup=vstup;
    if (vstup=="1b")
    {
        vystup="<span class=\"format\">"+vstup+"</span>";
    }
    if (vstup=="0a")
    {
        vystup="<span class=\"lf\">"+vstup+"</span>";
    }
    if (vstup=="0e")
    {
        vystup="<span class=\"escape\">"+vstup+"</span>";
    }

    return vystup;
}

short Soubor::vratCislo(char vstup)
{
    short vysledek=vstup&0xFF;
    return vysledek;
}
