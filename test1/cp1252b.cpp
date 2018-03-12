//#include <QtGui>
#include <QString>
//#include <QApplication>
//#include <QInputDialog>
#include <time.h>
//#include <math.h>
#include <QtGlobal>
#include <QtCore>
#include <QFile>



double a,b;

int main(int argc, char **argv) //
{
       // QCoreApplication ab(argc, argv);
        a=(double)clock();

        QTextCodec::setCodecForTr(QTextCodec::codecForName("CP-1251"));
        QSettings set("settings.ini",QSettings::IniFormat);

        QString str =set.value("Widget/DIR").toString();
        if (str!="")
            str+="/"+set.value("REZFILE").toString();
        else str = set.value("REZFILE").toString();
        QFile REZFILE(str);
        if (!REZFILE.open(QIODevice::Append))
            return(1);
        QTextStream REZ(&REZFILE);

        str = set.value("Widget/DIR").toString();
        if (str!="")
            str+="/"+set.value("LOGFILE").toString();
        else str = set.value("LOGFILE").toString();
        QFile LOGFILE(str);

        if (!LOGFILE.open(QIODevice::Append))
            return(1);
        QTextStream LOG(&LOGFILE);

        const quint8 tochn_null = set.value("TNULL").toUInt(); if (set.status()!=0) return(51);
        quint32 ndata = set.value("NDATA").toLongLong(); if (set.status()!=0) return(52);
        quint32 size = set.value("SIZE").toLongLong();if (set.status()!=0) return(53);
        double chis=0; if (set.status()!=0) return(54);

        const quint8 KOL_K = set.value("KOL_KANAL").toUInt(); if (set.status()!=0) return(55); // Количество каналов
        const quint8 N_ID_K = set.value("N_ID_KANAL").toUInt(); if (set.status()!=0) return(56);// N_ID_K - номер идеального канала;
        quint8 N_IZ_K,KAN[KOL_K]; if (set.status()!=0) return(57);//= set.value("N_IZ_KANAL").toUInt();// N_IZ_K - номер снимаевого канала;
        const quint8 debug = set.value("DEBUG").toUInt();

        qint16 NOLL; //в 0 идеальная в 1 измеряемая
        const quint16 M_KOL_P  = set.value("M_KOL_P").toUInt(); if (set.status()!=0) return(58);// максимальное количество переходов через 0
        qint16 extr[M_KOL_P];
        double Rab[M_KOL_P];
        bool znak,nachsin;
        const bool dvigOPOR = set.value("dvigOPOR").toBool(); if (set.status()!=0) return(59);
        quint32 i,j,k,l;
        double X;
        double N,K,T,min,max,maxP,minP,usrP,SDV,kB,aB,ku;
        const qint16 ERRORMAX=set.value("ERRORMAX").toInt(); if (set.status()!=0) return(510);
        const qint16 ERRORMIN=set.value("ERRORMIN").toInt(); if (set.status()!=0) return(511);
        KAN[1]=set.value("N_IZ_1_KANAL").toInt(); if (set.status()!=0) return(512);
        KAN[2]=set.value("N_IZ_2_KANAL").toInt(); if (set.status()!=0) return(513);
        KAN[3]=set.value("N_IZ_3_KANAL").toInt(); if (set.status()!=0) return(514);

        QDateTime dt = QDateTime::currentDateTime();
        LOG << dt.toString()<< "\r\n";

        QFile FILEDATA(set.value("FILE").toString()); if (set.status()!=0) return(515);
        if (!FILEDATA.open(QIODevice::ReadOnly)){
            LOG << QObject::tr("Файл данных ошибка чтения. Ошибка 3");
            LOGFILE.close();
            return(3);
        }
        else LOG << QObject::tr("Был открыт файл: ") << set.value("FILE").toString() << "\r\n"; if (set.status()!=0) return(515);
        //считывание из файла установки
        if (set.value("TEXT").toBool()){
            if (set.status()!=0) return(516);
            QTextStream FIL(&FILEDATA);
            FIL >> size;
            FIL >> chis;

        }
        else {
            QDataStream FIL(&FILEDATA);
            FIL.setVersion(set.value("QDATA_VER").toInt());
            if (set.status()!=0) return(517);
            FIL >> size;
            FIL >> chis;

        }
        qint16 DATA[size][KOL_K];
        if (set.value("TEXT").toBool()){
            if (set.status()!=0) return(516);
            QTextStream FIL(&FILEDATA);
            FIL.seek(0);
            FIL >> NOLL;
            FIL >> NOLL;
            for (i=0; i<ndata; i++)
                for (k=0; k<KOL_K; k++) FIL >> NOLL;
            for (i=0; i<size; i++){
                for (k=0; k<KOL_K; k++)FIL >> DATA[i][k];
                //if (i<30) LOG<<QString("%1 %2").arg(DATA[i][0]).arg(DATA[i][1])<< "\r\n";
               }



        }

        else {
            QDataStream FIL(&FILEDATA);
            FIL.setVersion(set.value("QDATA_VER").toInt());
            if (set.status()!=0) return(517);
            for (i=0; i<ndata; i++)
                for (k=0; k<KOL_K; k++) FIL >> NOLL;
            for (i=0; i<size; i++)
                for (k=0; k<KOL_K; k++) FIL >> DATA[i][k];
                    //if (i<3) LOG<<QString("%1").arg(DATA[i][k])<< "\r\n";}
                        };
        //LOGFILE.close();
        if (chis==0){
            LOG << QObject::tr("Неверный формат файла данных. Ошибка 9")<< "\r\n";
            LOGFILE.close();
            return(9);
        }
        else chis=chis/10;


        quint32 brac = quint32(size/1000); // процент выбраковки сигналов каждый 1000
        j=k=l=0;


        for (i=0; i<size; i++){
           if(set.value("INVERSION").toBool()){
            for (int ik=1; ik<KOL_K; ik++) //каналы для инверсии
               if (((DATA[i][ik]==ERRORMAX)or(DATA[i][ik]==ERRORMIN))and(set.value("f0").toBool())) DATA[i][ik]=(DATA[i][ik])^(-1);
            else
                DATA[i][ik]=-DATA[i][ik]; // инверсия сигналов кроме опорного
        //    REZ << QString("%1	").arg(DATA[i][2]);
         //  DATA[i][1]=-DATA[i][1];
        //   DATA[i][2]=-DATA[i][2];
        //    REZ << QString("%1	\r\n").arg(DATA[i][2]);
}
           if (!set.value("f0").toBool()){
            if ((j<=brac)&&((DATA[i][KAN[1]]==ERRORMAX)||(DATA[i][KAN[1]]==ERRORMIN))){
                j++;
            }
            if ((k<=brac)&&((DATA[i][KAN[2]]==ERRORMAX)||(DATA[i][KAN[2]]==ERRORMIN))){
                k++;
            }
            if ((l<=brac)&&((DATA[i][KAN[3]]==ERRORMAX)||(DATA[i][KAN[3]]==ERRORMIN))){
                l++;
            }

           /* if (j<=20){
                Rab[1]+=fabs(DATA[i][KAN[1]]);
            }
            if (k<=20){
                Rab[2]+=fabs(DATA[i][KAN[2]]);
            }
            if (l<=20){
                Rab[3]+=fabs(DATA[i][KAN[3]]);
            }*/


        if ((j<brac)||(KOL_K==2)) {
            ku=set.value("ku1").toDouble();
            N_IZ_K=set.value("N_IZ_1_KANAL").toInt();
        }
        if ((k<=brac)&&(j<brac)) {
            ku=set.value("ku2").toDouble();
            N_IZ_K=set.value("N_IZ_2_KANAL").toInt();
        }
        if ((l<brac)&&(k<brac)) {
            ku=set.value("ku3").toDouble();
            N_IZ_K=set.value("N_IZ_3_KANAL").toInt();
        }}
           else N_IZ_K=set.value("N_IZ_f0").toInt();
}
        LOG << QObject::tr("Номер идеального канала ")<< N_ID_K+1 << "\r\n";
        LOG << QObject::tr("Номер измеряемого канала ")<< N_IZ_K+1 << "\r\n";

        // определение знака начала синусоиды
        quint16 KOL_PPER=0, KOL_PER=0;
        for (i=0; i<M_KOL_P; i++)  extr[i]=0;
        T=0;
        quint32 porog = set.value("POROG").toInt(); //уровень изменения сигнала для срабатывания переключателя
        quint8 forma = (quint8)set.value("FORMA").toInt();

        nachsin=false;
/*-----------------------------------------------------------------------------------------------------------
        МЕАНДР
  -----------------------------------------------------------------------------------------------------------*/
        if (set.value("FORMA").toInt()==1){
            for (i=2; i<size; i++)
                if (((quint32)abs(DATA[i][N_ID_K]-DATA[i-1][N_ID_K])>porog)||((quint32)abs(DATA[i][N_ID_K]-DATA[i-2][N_ID_K])>porog))  {
                     if ((DATA[i][N_ID_K]<DATA[i-1][N_ID_K])&&(KOL_PPER==0)) nachsin=true;
                     Rab[KOL_PPER]=i-1;
                     //LOG << QString("%1	%2").arg(KOL_PPER).arg(Rab[KOL_PPER]) << "\r\n";

                     KOL_PPER++;
                     if (KOL_PPER>1000){
                             LOG << QObject::tr("Неверно настроен порог срабатывания. Ошибка 8")<< "\r\n";
                             LOGFILE.close();
                             return(8);
                         }
                     i+=tochn_null;
                 }
             if (KOL_PPER==0) {
                 LOG << QObject::tr("Нет опорного сигнала. Ошибка 6")<< "\r\n";
                 LOGFILE.close();
                 return(6);
             }
             KOL_PPER--;
             //LOG << QObject::tr("%1").arg(KOL_PPER)<< "\r\n";
             for (i=KOL_PPER;i>=1;i--) {
                 T+=(Rab[i]-Rab[i-1]);
                 //LOG << QString("%1	%2	%3").arg(T).arg(Rab[i]).arg(Rab[i-1]) << "\r\n";
                 if (((T/(KOL_PPER-i+1)+tochn_null)<(Rab[i]-Rab[i-1]))||((T/(KOL_PPER-i+1)-tochn_null)>(Rab[i]-Rab[i-1]))){
                     LOG << QObject::tr("Ошибка определения периода опорного сигнала(Точка %1 %2). Ошибка 7").arg(T/(KOL_PPER-i+1)).arg(Rab[i]-Rab[i-1])<<"\r\n";
                     LOGFILE.close();
                     return(7);
                 }
             }
             T=T*2/KOL_PPER;
             if (nachsin) N=Rab[0]; else N=Rab[1];
             if ((((KOL_PPER&1)==1)&&(nachsin))||(((KOL_PPER&1)==0)&&(!nachsin))) KOL_PPER--;
             K=Rab[KOL_PPER];
             KOL_PER=round((K-N)/T);

             //LOG << QString("%1 %2 ").arg(KOL_PER).arg(KOL_PPER) << "\r\n";


        }
/*-----------------------------------------------------------------------------------------------------------
                ИМПУЛЬСЫ
-----------------------------------------------------------------------------------------------------------*/
        if (set.value("FORMA").toInt()==2){

            for (i=0; i<size; i++){
                if (DATA[i][N_ID_K]>(int)porog)  {
                  //if ((DATA[i][N_ID_K]<DATA[i-1][N_ID_K])&&(KOL_PPER==0)) nachsin=true;
                  Rab[KOL_PER]=i;
                  if (debug==2)REZ << QString("%1 %2 %3").arg(i).arg(KOL_PER).arg(DATA[i][N_ID_K]) << "\r\n";
                  KOL_PER++;
                  if (KOL_PER>1000){
                          LOG << QObject::tr("Неверно настроен порог срабатывания. Ошибка 4")<< "\r\n";
                          LOGFILE.close();
                          return(4);
                      }
                  i+=tochn_null;

                }
            }
            if (KOL_PER==0) {
                LOG << QObject::tr("Нет опорного сигнала. Ошибка 6")<< "\r\n";
                LOGFILE.close();
                return(6);
            }
            KOL_PER--;

            for (i=KOL_PER;i>=1;i--) {
                T+=(Rab[i]-Rab[i-1]);
                //LOG << QString("%1	%2	%3").arg(T).arg(Rab[i]).arg(Rab[i-1]) << "\r\n";
            }

            T/=KOL_PER*2;
            N=qint32(Rab[0]);
            K=qint32(Rab[KOL_PER]);
            KOL_PER=round((K-N)/T);
            KOL_PPER=KOL_PER*2;

        }

        if (forma==0){ /*если форма не будет равна двум то надо будет проверить вычисление периода*/
        k=0;
        if ((DATA[k+tochn_null/2][N_ID_K]>0)&(DATA[k][N_ID_K]>0)) znak=true;
            else    if ((DATA[k+tochn_null/2][N_ID_K]<0)&(DATA[k][N_ID_K]<0)) znak=false;
                    else {
                        for (i=0; i<tochn_null; i++) LOG << DATA[i][N_ID_K]<< " ";
                        LOG << "\r\n" << QObject::tr("Возможна ошибка при определении знака") << "\r\n";
                        if ((DATA[k+tochn_null/2][N_ID_K]>0)&(DATA[k+tochn_null][N_ID_K]>0)) znak=false;
                        else    if ((DATA[k+tochn_null/2][N_ID_K]<0)&(DATA[k+tochn_null][N_ID_K]<0)) znak=true;
                                else LOG<<QObject::tr("Ошибка при определении знака")<<"\r\n";
                            }
        if (znak) LOG<< QObject::tr("Синусоида начинается с +") <<"\r\n";
        else  	LOG<< QObject::tr("Синусоида начинается с -") <<"\r\n";
        //

        maxP=ERRORMIN;
        minP=ERRORMAX;
        l=0; X=0;
        nachsin=false; // флаг определения начала синуса

        for (i=0; i<size; i++){
        if ((DATA[i][N_ID_K]>0)&&(!znak)) // прохождение положительной части синусоиды
                {
                j = i;//убрал -1
                for (k=i;(k<=i+tochn_null)&&(k<size);k++) if (DATA[k][N_ID_K]<=0) j=k;
                Rab[KOL_PPER]=(double)(i+j)/2;
                if (j==i) Rab[KOL_PPER]+=(double)DATA[i][N_ID_K]/(DATA[i-1][N_ID_K]-DATA[i][N_ID_K]);
                //LOG<<QString(" %1 %2").arg(i).arg(Rab[KOL_PPER])<< "\r\n";
                //if (KOL_PPER>2) LOG<<QString(" %1 %2").arg(Rab[KOL_PPER-2])<< "\r\n";
                i+=tochn_null;
                KOL_PPER++;
		znak=!znak;
                nachsin=true;
                };
        if ((DATA[i][N_ID_K]<0)&&(znak)&&(nachsin))	// прохождение отрицательной части синусоиды
		{
                j = i;//убрал -1
                for (k=i;(k<=i+tochn_null)&&(k<size);k++) if (DATA[k][N_ID_K]>=0) j=k;
                Rab[KOL_PPER]=(double)(j+i)/2;
                if (j==i) Rab[KOL_PPER]+=(double)DATA[i][N_ID_K]/(DATA[i-1][N_ID_K]-DATA[i][N_ID_K]);
                //LOG<<QString(" %1 %2").arg(i).arg(Rab[KOL_PPER])<< "\r\n";
                //if (KOL_PPER>2) LOG<<QString("%1").arg(Rab[KOL_PPER]-Rab[KOL_PPER-2])<< "\r\n";
                i+=tochn_null;
                KOL_PPER++;
		znak=!znak;
                };
       if ((DATA[i][N_ID_K]<0)&&(znak)&&(!nachsin)){
           znak=!znak;
           nachsin=true;
           i+=tochn_null;

       }
       if ((znak)&&(DATA[i][N_ID_K]>extr[KOL_PPER]))  extr[KOL_PPER]=DATA[i][N_ID_K];
       if ((!znak)&&(DATA[i][N_ID_K]<extr[KOL_PPER])) extr[KOL_PPER]=DATA[i][N_ID_K];
       //if (DATA[i][N_IZ_K]>maxP) maxP=DATA[i][N_IZ_K];
       //if (DATA[i][N_IZ_K]<minP) minP=DATA[i][N_IZ_K];
       //X+=DATA[i][N_IZ_K];
       if ((DATA[i][N_IZ_K]==ERRORMAX)||(DATA[i][N_IZ_K]==ERRORMIN)){
           LOG << QObject::tr("Ошибка 5.Возможна ошибка. Выход за границу измерений. Измеряемой синусоиды.")<< QString(" %1").arg(l)<<"\r\n";
           l++;
       }
       if ((DATA[i][N_ID_K]==ERRORMAX)||(DATA[i][N_ID_K]==ERRORMIN)){
           LOG << QObject::tr("Ошибка 5.Возможна ошибка. Выход за границу измерений. Идеальной синусоиды.")<< QString(" %1").arg(l)<<"\r\n";
           l++;
       }

       if (l>20){
           LOGFILE.close();
           return(5);
       }
       };
      /* if ((maxP==0)||(minP==0)){
            LOG << QObject::tr("Ошибка 4.Синусоида сильно смещена относительно 0.");
            LOGFILE.close();
            return(4);
        }*/
       KOL_PPER--;// один лишний и два для движениия синуса
       if (dvigOPOR) KOL_PPER-=4;
       for(KOL_PPER;(KOL_PPER&(set.value("KKP").toUInt()-1))!=0;KOL_PPER--){};
       T=0;
       max=min=0;
       for (i=0;i<=KOL_PPER;i++) LOG << Rab[i] << "\r\n";
       for (i=KOL_PPER;i>=1;i--)
       {
           T+=(Rab[i]-Rab[i-1]);
            if (extr[i]>0) max+=extr[i];
            if (extr[i]<0) min+=extr[i];
       };
       KOL_PER=KOL_PPER/2;
       max/=KOL_PER;
       min/=KOL_PER;
       KOL_PER--;
       for (i=KOL_PER;i>=1;i--) T+=(Rab[i]-Rab[i-1]);


             T/=KOL_PER;
             N=Rab[0];
             K=Rab[KOL_PER];


               //LOG << QString("%1       ").arg(DATA[i][N_ID_K]);

   }



        qint16 SIN[size+quint16(2*T)];

        for (i=0; i<(size+quint16(2*T)); i++) {
            //if (i<size) LOG << QString("%1	%2\r\n").arg(DATA[i][N_ID_K]).arg(DATA[i][N_IZ_K]);
        if (forma!=0){

            SIN[i]=10000*sin((2*M_PI/T)*(i-N));

        }
        else
            if (i<size) SIN[i]=DATA[i][N_ID_K];
        };

        //LOGFILE.close();
        usrP=0;
        if(set.value("SDVIG").toBool())
            for (i=N; i<=K; i++)
                usrP+=DATA[i][N_IZ_K];

        usrP/=K-N+1;//X/size;
        //LOG<<QObject::tr("Сдвиг ид.")<< usr << "\r\n";
        LOG<<QObject::tr("Сдвиг Измер.")<< usrP << "\r\n";


        LOG<<QString("T %1 %2").arg(T/(chis*1000),0,'f',5).arg(T)<< "\r\n";
        LOG<<QString("MAX %1").arg(max,0,'f',5) << "\r\n";
        LOG<<QString("MIN %1").arg(min,0,'f',5) << "\r\n";
        LOG<<QString("NACHALO %1").arg(N,0,'f',5) << "\r\n";
        LOG<<QString("KONEZ %1").arg(K,0,'f',5) << "\r\n";
        LOG<<QString("KOL-VO periodov %1").arg(KOL_PER) << "\r\n";

        //set.value(QString("Phase/%1").arg(round((chis*1000)/T))).toDouble();

        //for (i=0; i<size; i++)   LOG << QString(" %1").arg(DATA[i][N_ID_K])<< "\r\n";
        double F,A;
        bool period, konez, sdvig;
        double LPeremn,PPeremn,NLPeremn,NPPeremn,Peremn1,Peremn2;
        /* LPeremn - левое перемножение
           NLPeremn - номер левого перемножения
           PPeremn - правое перемножение
           NPPeremn - номер правого перемножения
           Peremn1- первое перемножение
           Peremn2- второе перемножение
*/


        //double POPRAVKA =set.value("NACH_OTST").toDouble();
        //использование поправки надо переделать
        double POPRAVKA;
       // if (set.value("Phase/0").toChar()=='q')
            POPRAVKA = set.value(QString("Phase/%1").arg(round((chis*10*1000)/T))).toDouble();
       // else
         //   POPRAVKA = set.value("Phase/0").toDouble();

        LOG << QString("POPRAVKA %1").arg(POPRAVKA) << "\r\n";
        SDV=0;

   //     do


                i=0;
                konez=true;
                period=true;
                sdvig=false;
                do
                {
                    X=0;//сумма перемножений
                    j=round(N);
                    //NP=N-round(N);
                 do
                 {

                     //if (k>K) k+=round(N)-round(K)-1;
                     if (dvigOPOR) X+=(DATA[j+i][N_IZ_K]-usrP)*SIN[j]/(K-N);
                         else X+=(DATA[j][N_IZ_K]-usrP)*SIN[j+i]/(K-N);
                    //if (period) LOG << QString("# %1 %2 %3").arg(j).arg(DATA[j+i][N_IZ_K]-usrP).arg(SIN[j]) << "\r\n";
                     j++;
                     //k++;

		 }
                 while(j<round(K));
               if (set.value("DEBUG").toInt()==3){
                   REZ << QString("	%1	%2").arg(i).arg(X,0,'f',0) << "\r\n";


               }
             //   i++;

                if (set.value("CXEMA").toUInt()==1){

                  if (!period){
                    if (sdvig) {
                        //LPeremn=X;
                        //A=2*(LPeremn+(F-quint32(F))*(PPeremn-LPeremn))/10000;
                        Peremn2=X;
                        A=2*sqrt(pow(Peremn1,2)+pow(Peremn2,2))/10000;
                        konez=false;
                        //F+=POPRAVKA;
                    }
                    if (!sdvig) {
                        //PPeremn=X;
                        Peremn1=X;
                        //i-=1;
                        i+=round(T/4);
                        sdvig=true;


                    }

                     }
                  else {
                  if ((i==0)) {
                      LPeremn=X;
                      NLPeremn=i;
                      i=round(T/2);
                  } else if (i==round(T/2)) {
                      PPeremn=X;
                      NPPeremn=i;
                      i=round(T/4);
                  } else if ((i!=0)&&(i!=round(T/2))){
                      if (X*LPeremn>0) {
                          LPeremn=X;
                          NLPeremn=i;
                          i=round(i+(NPPeremn-i)/2);
                      } else {
                          PPeremn=X;
                          NPPeremn=i;
                          i=round(i+(NLPeremn-i)/2);
                      }
                  }

                if (((NPPeremn-NLPeremn)==1)){
                        F=(NLPeremn+(LPeremn/(LPeremn-PPeremn)));
                      // if (dvigOPOR) F-=T/4;
                      // else F+=T/4;
                     //  LOG << QString("# %1").arg(F) << "\r\n";
                        period=false;

                       if (F>0) i=quint32(F)+1;
                        else i=quint32(T+F+1);

                        if ((LPeremn>0)&(PPeremn<0))
                        {
                            F-=T/4;
                            //if (!set.value("f0").toBool()) if (F<0) F+=T; //для F0 закрыть

                        }
                        if ((LPeremn<0)&(PPeremn>0))
                        {
                            F+=T/4;
                            //if (!set.value("f0").toBool()) if (F<0) F+=T; //для F0 закрыть
                            //if (F>0) F-=T;

                        }
                      // LOG << QString("# %1").arg(F) << "\r\n";

                    }
            }

            }
                  if (set.value("CXEMA").toUInt()==2){
                     if (i!=0){
                         Peremn2=X;
                         F=T*atan(Peremn1/Peremn2)/(M_PI*2);
                         A=2*sqrt(pow(Peremn1,2)+pow(Peremn2,2))/10000;
                         //LOG << QString("# %1 /%2").arg(F).arg(A) << "\r\n";
                         if (F<0) F+=T;
                         konez=false;

                     }
                     if (i==0){
                         Peremn1=X;
                         i-=round(T/4);

                     }
                 }
                //if (i==qint16(T)) konez=false;
            }
                while (konez);//&&(l!=10)
                if (set.value("DEBUG").toInt()==1){
                    qint16 SIN_f[size+quint16(2*T)];
                    for (i=0; i<size; i++) {
                     SIN_f[i]=A*sin((2*M_PI/T)*(i-(N+F)));//-POPRAVKA/360*T
                     if (i<size) REZ << QString("%1	%2	%3	%4\r\n").arg(SIN[i]).arg(DATA[i][N_IZ_K]).arg(DATA[i][N_ID_K]).arg(SIN_f[i]);
                    }
                }
                double T_ras= T/(1000*chis);
                double F_ras= (F/T)*2*M_PI;
                if (!set.value("f0").toBool()){
                 POPRAVKA = POPRAVKA/360*2*M_PI;
                 F_ras-=POPRAVKA;
                }
                //LOG << QObject::tr("ФАЗА ") << QString("%1 ").arg(F,0,'f',5)<< "\r\n";
                //if (!set.value("f0").toBool())REZ << QString("ЧАСТОТА	| АМПЛИТУДА	| КАППА	| Температуропроводность 10-6").arg(A/ku,0,'f',9);

                LOG << QObject::tr("АМПЛИТУДА ") << QString(" %1 ").arg(A/ku,0,'f',6)<< "\r\n";
                //LOG<<QObject::tr("Амплитуда ")<< (2*X)/((max-min)*(K-N+1)) <<"\r\n";

                LOG << QObject::tr("ФАЗА ") << QString("%1 %2").arg(F_ras,0,'f',5).arg(F)<< "\r\n";
                REZ << QString("%L1").arg((chis*1000)/T,0,'f',1);
                if (set.value("f0").toBool()) REZ << QString("=%1	%1").arg(F/T*360,0,'f',9);

                LOG << QObject::tr("Поправка ") << QString("%1 ").arg(POPRAVKA,0,'f',5)<< "\r\n";

                //kB=M_SQRT2*(M_PI*2*(F/(chis*1000)-POPRAVKA/360*T)*(chis*1000)/T-M_PI/4);
                kB=M_SQRT2*(F_ras-M_PI/4);
                LOG << QObject::tr("каппа ") << QString("%1 ").arg(kB,0,'f',9)<< "\r\n";

                aB=(M_PI*2*pow(0.000001*set.value("l").toUInt(),2))/(T_ras*pow(kB,2));
                LOG << QObject::tr("температуропроводность ") << QString("%1 ").arg(aB,0,'E',12)<< "\r\n";

                if (!set.value("f0").toBool())REZ << QString("	%L1	%L2	%L3	%L4	%L5	%L6").arg(A/ku,0,'f',9).arg(kB,0,'f',9).arg(aB,0,'E',4).arg(F/T*360,0,'f',9).arg(F).arg(F/(1000*chis));

                //LOG << QObject::tr("не каппа ") << QString("%1 ").arg(kB,0,'f',9)<< "\r\n";
                /*qint16 SIN2[size+quint16(2*T)];
                for (i=0; i<(size+quint16(2*T)); i++){
                SIN2[i]=-10000*sin((2*M_PI/T)*(i-N-F+POPRAVKA*chis*1000));
                if (i<size) REZ << QString("%1	%2	%3\r\n").arg(SIN[i]).arg(SIN2[i]).arg(DATA[i][N_IZ_K]);
                }*/

     /*          F+=T;

               // LOG << QObject::tr("ФАЗА + T ") << QString("%1 ").arg(F/(chis*1000),0,'f',5)<< "\r\n";
                LOG << QObject::tr("ФАЗА +T ") << QString("%1 ").arg(F/(chis*1000)-POPRAVKA,0,'f',5)<< "\r\n";
                kB=M_SQRT2*(M_PI*2*(F/(chis*1000)-POPRAVKA)*(chis*1000)/T-M_PI/4);
                //REZ << QString("%1	").arg(kB,0,'f',9);
                LOG << QObject::tr("каппа +T ") << QString("%1 ").arg(kB,0,'f',9)<< "\r\n";
                aB=(M_PI*2*pow(0.000001*set.value("l").toUInt(),2)*chis*1000)/(T*pow(kB,2));
                //REZ << QString("%1	").arg(aB,0,'f',12);
                LOG << QObject::tr("температуропроводность +T ") << QString("%1 ").arg(aB,0,'f',12)<< "\r\n";
                //if (F<0 ) F+=T/(chis*1000);
                //F+=T/(chis*1000);*/


              /* // LOG << QObject::tr("АМПЛИТУДА ") << QString("%1 ").arg(A/ku,0,'f',6)<< "\r\n";
                kB=M_SQRT2*(M_PI*2*(F/(chis*1000)-POPRAVKA)*(chis*1000)/T-M_PI/4);
                //LOG << QObject::tr("не каппа ") << QString("%1 ").arg(kB,0,'f',9)<< "\r\n";
                fB=kB*T/((sqrt(pow(sinh(kB*M_SQRT1_2),2)+pow(sin(kB*M_SQRT1_2),2)))*(A/ku)*M_PI*2*chis*1000);
                //LOG << QObject::tr("относительная теплоемкость ") << QString("%1 ").arg(fB,0,'f',9)<< "\r\n";
                 LOG << QObject::tr("каппа ") << QString("%1 ").arg(kB,0,'f',9)<< "\r\n";
                aB=(M_PI*2*pow(0.000001*set.value("l").toUInt(),2)*chis*1000)/(T*pow(kB,2));
                LOG << QObject::tr("температуропроводность ") << QString("%1 ").arg(aB,0,'f',12)<< "\r\n";
                //LOG << QString("%1 ").arg(NP)<< "\r\n";
               // SDV+=T;*/
                

       // while(SDV<=(T));
        //F*=(-1);
        LOG << QObject::tr("СХЕМА ") << QString("%1 \r\n").arg(set.value("CXEMA").toUInt());
        j=0;
        LOG << "*"<<"\r\n";
        b=(double)clock();
        LOG << (b-a)/CLOCKS_PER_SEC << "\r\n";
        LOG << "*"<<"\r\n";
        REZ <<"\r\n";
        FILEDATA.close();
        LOGFILE.close();
        REZFILE.close();
        return (0);
}
