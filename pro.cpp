#include <mpi.h>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>

using namespace std;

#define TAG 0
int velikostVstupu;
void odeslatNaProcesor (int prvek, int inverzni, int naslednik, int predchudce, char uzel, int predchudce2Posilany)
{
//	std::cout << "odesílám na procesor hranu: "<< prvek << ", s inverzní hranou: "<< inverzni << ", s následníkem: " << naslednik << " a s předchůdcem: "<< predchudce<< std::endl;
	MPI_Send(&inverzni, 1, MPI_INT, prvek, TAG, MPI_COMM_WORLD);
	MPI_Send(&naslednik, 1, MPI_INT, prvek, TAG, MPI_COMM_WORLD);
	MPI_Send(&predchudce, 1, MPI_INT, prvek, TAG, MPI_COMM_WORLD);
	MPI_Send(&uzel, 1, MPI_CHAR, prvek, TAG, MPI_COMM_WORLD);
	MPI_Send(&predchudce2Posilany, 1, MPI_INT, prvek, TAG, MPI_COMM_WORLD);
	MPI_Send(&velikostVstupu, 1, MPI_INT, prvek, TAG, MPI_COMM_WORLD);
}

int main(int argc, char *argv[])
{
	
	int processors; // počet procesorů
	int myId = 0; // číslo procesoru
	int prvek = 0;

	int inverzni;
	int naslednik;
	int naslednikPosilany;
	int predchudce;
	int predchudcePosilany;
	int predchudce2;
	int predchudce2Posilany;
	int vaha;
	int vahaPosilany;
	int poradi;
	int preorder;
	char uzel;
	int vzdalenost = 1;
	double start, end;
	string vstup;
	std::vector<int> hranaRodic;
	std::vector<int> iHranaRodic;
	std::vector<int> hranaLPotomek;
	std::vector<int> iHranaLPotomek;
	std::vector<int> hranaRPotomek;
	std::vector<int> iHranaRPotomek;
	std::vector<char> poleUzlu;
	std::vector<char> finalniPole;
	std::vector<int> finalniPoradi;

	MPI_Status stat;
	int eTour;

    //MPI INIT
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processors);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);
	
	if (myId == 0){

		vstup = argv[1];
		vector<char> vstupniPole(vstup.begin(), vstup.end());
		velikostVstupu = vstupniPole.size();
//		std::vector<char>::iterator it;
//		it = vstupniPole.begin();
//		it = vstupniPole.insert(it, '0');
		for(int i = 0; i< vstupniPole.size();i++){ //první průchod, vyplnění tabulek, kromě inverzních hran potomků.
//			std::cout << "hodnota = "<<vstupniPole[i]<< std::endl;
			int lPotomek = (((i+1)*2)-1);
			int rPotomek = ((i+1)*2);
			if(i!=0){
				int rodic = ((i+1)/2)-1;
				bool LPotomek = false;
				if(i%2 != 0){
					LPotomek = true;
				}
				hranaRodic.push_back(prvek);
	//			std::cout <<"prvek = "<< i<< std::endl;
	//			std::cout <<"velikost pole = "<< hranaRodic.size()<< std::endl;
	//			std::cout << hranaRodic[i]<< std::endl;
	//			for(int u =0; u <hranaRodic.size(); u++){
	//				std::cout << hranaRodic[u]<< std::endl;
	//			}
				if(LPotomek){
					iHranaRodic.push_back(hranaLPotomek[rodic]);		
				}else{
					iHranaRodic.push_back(hranaRPotomek[rodic]);		
				}
	//		std::cout << "od uzlu "<< i << " do uzlu "<< rodic << " vede hrana č. " << prvek <<". Její inverzí je hrana č. "<< iHranaRodic[i] << "."<< std::endl;
			prvek++;	
			}	
			else{
				hranaRodic.push_back(-1);
				iHranaRodic.push_back(-1);
			}		
			if(lPotomek<vstupniPole.size()){
				hranaLPotomek.push_back(prvek);
		//		std::cout << "od uzlu "<< i << " do uzlu "<< lPotomek << " vede hrana č. " << prvek << "."<< std::endl;
				prvek++;

				if(rPotomek<vstupniPole.size()){
					hranaRPotomek.push_back(prvek);
		//			std::cout << "od uzlu "<< i << " do uzlu "<< rPotomek << " vede hrana č. " << prvek << "."<< std::endl;
					prvek++;
				}
			}
				
			
		}
		prvek=0;
		for(int i = 0; i< vstupniPole.size();i++){ //2. doplnění tabulky inverzními hranami u potomků + odeslání na procesory
			
			int lPotomek = (((i+1)*2)-1);
			int rPotomek = ((i+1)*2);
			
			if(lPotomek<vstupniPole.size()){
				iHranaLPotomek.push_back(hranaRodic[lPotomek]);

				if(rPotomek<vstupniPole.size()){
					iHranaRPotomek.push_back(hranaRodic[rPotomek]);
				}
			}
			if(i!=0){
				prvek = hranaRodic[i];
				inverzni = iHranaRodic[i];
				predchudce = prvek;
				if(lPotomek<vstupniPole.size()){
					naslednik = hranaLPotomek[i];				
				}else{
					naslednik =-1;
				}
				char prazdny ='\0';
				if(rPotomek<vstupniPole.size()){
		//			std::cout <<"bod: "<< i <<", hrana: "<< prvek<< ", inverzní hrana k pravému potomku: "<< iHranaRPotomek[i] << std::endl;
					predchudce2Posilany = iHranaRPotomek[i];
				}else if(lPotomek<vstupniPole.size()){
		//			std::cout << i << ", inverzní hrana k levému potomku: "<< iHranaRPotomek[i] << std::endl;
					predchudce2Posilany = iHranaLPotomek[i];
				}
				odeslatNaProcesor(prvek, inverzni, naslednik, predchudce, prazdny, predchudce2Posilany);
			}
			if(lPotomek<vstupniPole.size()){
				predchudce2Posilany = -2;
				prvek = hranaLPotomek[i];
				inverzni = iHranaLPotomek[i];				
				if(i==0){
					predchudce = prvek;
				}
				if(rPotomek<vstupniPole.size()){
					naslednik = hranaRPotomek[i];				
				}else{
					naslednik =-1;
				}
				
				predchudce2Posilany = iHranaRodic[i];
				
				odeslatNaProcesor(prvek, inverzni, naslednik, predchudce, vstupniPole[(((i+1)*2)-1)], predchudce2Posilany);
			}
			if(rPotomek<vstupniPole.size()){
				predchudce2Posilany = -2;
				prvek = hranaRPotomek[i];
				inverzni = iHranaRPotomek[i];	
				naslednik = -1;
				
				predchudce2Posilany = iHranaLPotomek[i];
				
				odeslatNaProcesor(prvek, inverzni, naslednik, predchudce, vstupniPole[((i+1)*2)], predchudce2Posilany);
			}
		}
	start = MPI_Wtime(); // začátek měření
	}
	MPI_Recv(&inverzni, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
	MPI_Recv(&naslednik, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
	MPI_Recv(&predchudce, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
	MPI_Recv(&uzel, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
	MPI_Recv(&predchudce2Posilany, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
	MPI_Recv(&velikostVstupu, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
//	std::cout << "odesílám na procesor hranu: "<< myId << ", s inverzní hranou: "<< inverzni << ", s následníkem: " << naslednik << " a s předchůdcem: "<< predchudce<< std::endl;
	naslednikPosilany = naslednik;
	predchudcePosilany = predchudce;

	predchudce2 = -2;
	if(myId==0){
		predchudce2 = -1;
	}
	if(predchudce == myId && naslednik == -1){
		predchudce2 = inverzni;
	}

	if(predchudce2 == -2){
		predchudce2 = predchudce2Posilany;
	}
//	std::cout << "procesor: "<< myId << ", předchůdce: "<< predchudce2<<", následník: " <<naslednik<<std::endl;

	MPI_Send(&naslednikPosilany, 1, MPI_INT, inverzni, TAG, MPI_COMM_WORLD);
	MPI_Send(&predchudcePosilany, 1, MPI_INT, inverzni, TAG, MPI_COMM_WORLD);
	MPI_Recv(&naslednikPosilany, 1, MPI_INT, inverzni, TAG, MPI_COMM_WORLD, &stat);
	MPI_Recv(&predchudcePosilany, 1, MPI_INT, inverzni, TAG, MPI_COMM_WORLD, &stat);
//	std::cout << "procesor: "<< myId << ", následník jeho inverzní hrany je: "<< naslednikPosilany<< std::endl;

	if(naslednikPosilany != -1){
		naslednik = naslednikPosilany;
	}else{
		naslednik = predchudcePosilany;
	}
	if(naslednik ==0){
		naslednik = -1;
	}
//	std::cout << "procesor: "<< myId << ", předchůdce: "<< predchudce2<<", následník: " <<naslednik<<std::endl;
	if(uzel!='\0'){
		vaha = 1;
	}else{
		vaha = 0;
	}
	predchudce = predchudce2;
	int ssDokonceno = 0;
	while(vzdalenost < processors){
	
		vzdalenost = vzdalenost*2;
		
		naslednikPosilany = naslednik;
		predchudcePosilany = predchudce;
		vahaPosilany = vaha;
		if(predchudce!=-1){
			MPI_Send(&vahaPosilany, 1, MPI_INT, predchudce, TAG, MPI_COMM_WORLD);
			MPI_Send(&naslednikPosilany, 1, MPI_INT, predchudce, TAG, MPI_COMM_WORLD);
//			std::cout << "jsem procesor: "<<myId<<", poslal jsem údaje na předchůdce: "<<predchudce<<std::endl;
		}
		if(naslednik!=-1){
			MPI_Recv(&vahaPosilany, 1, MPI_INT, naslednik, TAG, MPI_COMM_WORLD, &stat);
			MPI_Recv(&naslednikPosilany, 1, MPI_INT, naslednik, TAG, MPI_COMM_WORLD, &stat);
			MPI_Send(&predchudcePosilany, 1, MPI_INT, naslednik, TAG, MPI_COMM_WORLD);
//			std::cout << "jsem procesor: "<<myId<<", přijal/poslal jsem na následníka : "<<naslednik<<std::endl;
			vaha = vahaPosilany + vaha;
			naslednik = naslednikPosilany;

		}
//		std::cout << "cyklus"<<std::endl;
		if(predchudce!=-1){
			MPI_Recv(&predchudcePosilany, 1, MPI_INT, predchudce, TAG, MPI_COMM_WORLD, &stat);
			predchudce = predchudcePosilany;
		}


//		std::cout << "test, došlo ke konci, procesor: "<<myId<<std::endl;
/*		if(myId ==0){
			if(naslednik == -1){
				ssDokonceno = 1;
			}
			
			for(int i=0; i<processors; i++){
				MPI_Send(&ssDokonceno, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
			}
		}
		MPI_Recv(&ssDokonceno, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);*/

	}
//	std::cout << "cyklus dokončen"<<std::endl;
	
//	std::cout << "jsem procesor: " << myId << ", moje váha je: "<< vaha << ", můj uzel: "<<uzel<<std::endl;
	if(uzel!='\0'){
		preorder = velikostVstupu - vaha;
	//	std::cout << "jsem procesor: " << myId << ", preorder je: "<< preorder <<std::endl;
	}
	MPI_Send(&preorder, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
	MPI_Send(&uzel, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
	//std::cout <<"procesor: "<<myId<< " zasílá na řídící procesor"<<std::endl;


	if(myId ==0){
		end = MPI_Wtime(); // ukončení měření času
//		std::cout << "vstup: " << vstup <<std::endl;
		finalniPole.insert(finalniPole.begin(), vstup.at(0));
		for(int i=0; i<processors; i++){
			MPI_Recv(&preorder, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat);
			MPI_Recv(&uzel, 1, MPI_CHAR, i, TAG, MPI_COMM_WORLD, &stat);
	//		std::cout << finalniPole.size()<<", preorder: "<<preorder<<", uzel: "<<uzel<<std::endl;
			finalniPoradi.insert(finalniPoradi.begin()+i, preorder);
			
			poleUzlu.insert(poleUzlu.begin()+i, uzel);
			
		}
//		std::cout << "velikost pole = "<< finalniPole.size()<<std::endl;
		int index =1;
		while(finalniPole.size()!=velikostVstupu){
	//		std::cout << "velikost pole = "<< finalniPole.size()<<std::endl;
			for(int i=0; i<finalniPoradi.size(); i++){
	//			std::cout << "i = "<< i<<", preorder požadován: "<<index<<", uzel: "<<", preorder ička: "<< finalniPoradi[i]<<std::endl;
				if(finalniPoradi[i]==index){
					finalniPole.push_back(poleUzlu[i]);	
			//		std::cout << "podmínka splněna, přidán uzel = "<< poleUzlu[i]<<std::endl;
					index++;
				}				
			}			
		}
		
//		std::cout << finalniPole[0];
		for(int i=0; i<finalniPole.size();i++){
//			std::cout << finalniPole[i];
			
		}
//		cout << endl;
		cerr << end-start << endl;
	}

	MPI_Finalize();
    return 0;
}
