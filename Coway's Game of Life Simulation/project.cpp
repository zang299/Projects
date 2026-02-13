// Name: Thian Zhang Hao
// Project driver file 
// Date: 4-5-2024
// Description of program: Program used to simulate the game of life of the animal pig using many functions and class




#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <string>
#include "pig.h"
#include <fstream>
using namespace std;


	
void bornNewPig(vector<Pig>& pigs) 
// creates function called bornNewPig used to give birth to new pig objects and takes in the pigs vector object as parameter
{
    int maleCount = 0;
    int femaleCount = 0;
	// creates two variables used for counting number of males and females 
    for (int i=0; i<pigs.size(); i++) {
        if (pigs[i].getGender() == 1 && pigs[i].getAge() >= 2 && !pigs[i].getmarvel()) {
            femaleCount++;
        } else if (pigs[i].getGender() == 2 && pigs[i].getAge() >= 2 && !pigs[i].getmarvel()) {
            maleCount++;
        }
    }
	// counts the number of males and females that are within child bearing age and not marvel 


    if (maleCount > 0 && femaleCount > 0) 
	// runs the statements below if maleCount and femaleCount is above 0
	{
        for (int i = 0; i < pigs.size(); i++) 
		// iterates through all the elements in the pigs vector 
		{
			int femaleIndex = -1;
			// sets a variable femaleIndex used for storing the mother's index to -1 at first 
            if (pigs[i].getGender() == 1 && pigs[i].getAge() >= 2 && !pigs[i].getmarvel()) 
			// finds the females that can be mothers through the same two conditions for calculating number of females in femaleCount 
			{
                femaleIndex = i;
				// sets the specific index of the mother found at first to the femaleIndex variable 
				pigs.push_back(Pig(pigs[femaleIndex].getcolor()));
				// creates a new pig object in the pigs vector that takes in the same color as their mother by putting the femaleIndex in their constructor 
				cout<<"Piggy "<<pigs[pigs.size()-1].getname()<<" is born!"<<endl;
				// tells the user the name of the pig object that was just created as a new piggy that was born
				femaleCount--;
				// decrements the femaleCount by 1 as the loop iterates 
            } 
            
		
			if (femaleCount == 0) 
               break;
			// when the femaleCount reaches 0 it will break out of the for loop
			
		}
	}	
}


void pigdie(vector<Pig>& pigs)
// creates a void function pigdie that is used for pigs that are way too old (age 8) to pass away and takes in the pigs vector as parameter
{
	int n=0;
	// first declares a variable called n to 0 
	while(n<pigs.size())
	// constantly runs the loop as long as n is less than the size of the pigs vector
	{
		if(pigs[n].getmarvel()==false)
		{
			if(pigs[n].getAge() ==8)
			{
				cout<<"Piggy "<<pigs[n].getname()<<" has passed away"<<endl;
				pigs.erase(pigs.begin()+n);
			}
			// when non marvel pigs reach the age of 8, they will be erased from the vector and it will tell the user the name of the pig that passed away 
			else
				n++;
			// or else it will increment n by 1 which means going to the next element and loop again 
		}
		else
		{
			if(pigs[n].getAge()==20)
			{
				cout<<"Piggy "<<pigs[n].getname()<<" has passed away"<<endl;
				pigs.erase(pigs.begin()+n);
			}
			// when marvel pigs reach the age of 20, they will be erased from the vector and it will tell the user the name of the pig that passed away 
			else 
				n++;
			// or else it will increment n by 1 which means going to the next element and loop again 
		}
	}
}

void passMarvelPower(vector<Pig>& pigs)
// creates a void function pigdie that is used to pass the marvel power to a random pig the next year and takes in the pigs vector as parameter
{
	int ran;
	vector<int> marvelindex;
	// creates an integer ran used to store the random number and a vector called marvelindex used to store the index of the pigs with marvel
	for(int i=0; i<pigs.size(); i++)
	{
		if(pigs[i].getmarvel())
			marvelindex.push_back(i);
		// when the pig has marvel, it will add its specific index to the marvelindex as a new element 
	}
	
	if(marvelindex.empty()==false)
	// runs the statements below when the marvelindex contains at least one element (there is at least one marvel pig)
	{
		int marvelcount = marvelindex.size();
		// sets an integer variable marvelcount to the size of the marvelindex vector 
		
		vector<int> nonmarvelindex;
		// creates a new vector called nonmarvelindex used to store pigs that dont have marvel 
		for(int i=0; i<pigs.size(); i++)
		{
			if(pigs[i].getmarvel()==false)
				nonmarvelindex.push_back(i);
		}
		// stores pigs that dont have marvel in the non marvel index
		
		for(int i=0; i<marvelindex.size(); i++)
		{
			ran=rand()%nonmarvelindex.size();
			// sets ran to a random number that is in the range of the indexes in the nonmarvelindex 
			pigs[nonmarvelindex[ran]].setmarvel();
			// picks out a random index of the nonmarvelindex which contains an index from the pigs vector and sets it as marvel
			nonmarvelindex.erase(nonmarvelindex.begin()+ran);
			// it then erases it from the nonmarvelindex 
			if(nonmarvelindex.empty())
				break;
			// when the nonmarvelindex is empty, it will break out of the loop 
		}
	}
}
			
		
		
			





int main()
{
	srand(time(0));
	int years=0;
	// sets years as 0 at first 
	
	vector<Pig> pigs(5);
	// creates 5 pig objects in pigs vector at first
	
	ofstream outputFile("output.txt"); 
	// creates and opens a new txt file 

    streambuf* coutbuf = cout.rdbuf(); 
	// saves the original cout buffer
    cout.rdbuf(outputFile.rdbuf());
	// causes any output to the console to be redirected to the file output.txt 
	
	for(int i=0; i<5; i++)
	{
		pigs[i]=Pig(i%5+1);
	}
	// sets the first five pigs to each of them having the 5 colors respectively 
	
	char killtoken;
	// creates a character variable called killtoken used to store the userinput 
	
	while(pigs.size()>0 && pigs.size()<200)
	{	
		cout<<"Year: "<<years<<endl;
		cout<<endl;
		
		pigdie(pigs);
		
		cout<<endl;
		
		bornNewPig(pigs);
		
		cout<<endl;
		
		if(years!=0)
			passMarvelPower(pigs);
		
		// runs all the birth of pigs, death of pigs, and the passing of marvel of pigs functions 
		
		for(int i=0; i<pigs.size(); i++)
		{
			cout<<"Name:"<<pigs[i].getname()<<endl;
			switch (pigs[i].getcolor())
			
			{
			case 1: 
				cout<<"Color: "<<"White"<<endl;
				break;
			case 2: 
				cout<<"Color: "<<"Brown"<<endl;
				break;
			case 3: 
				cout<<"Color: "<<"Grey"<<endl;
				break;
			case 4: 
				cout<<"Color: "<<"Black"<<endl;
				break;
			case 5: 
				cout<<"Color: "<<"Blonde"<<endl;
				break;
			}
			cout<<"Age:"<<pigs[i].getAge()<<endl;
			if(pigs[i].getmarvel())
				cout<<"Marvel: True"<<endl;
			else
				cout<<"Marvel: False"<<endl;
			
			if (pigs[i].getGender()==1)
				cout<<"Gender: Female"<<endl;
			else
				cout<<"Gender: Male"<<endl;
			cout<<endl;
			
		}
		// prints out each pig objects information from the pigs vector at that specific year
			
		
		
		for(int i = 0; i < pigs.size(); i++) 
		{
			pigs[i].incrementAge();
		}
		// increments every pig's age 
         
		cout.rdbuf(coutbuf);
		cout <<"Year "<<years<<": Before proceeding to the next year, do you want to kill half of the population? (Enter 'K' for Yes, any other key to continue): ";
		cin >> killtoken;
		// asks the user if the user wants to kill half the population 

		if(killtoken == 'K' || killtoken == 'k') 
		{
			
			vector<int> halfindex;
			// creates an index called halfindex used to store the indexes removed for the population kill 
			if(pigs.size()==1)
			{
				pigs.erase(pigs.begin());
				cout<<"Successfully killed remaining population."<<endl;
				// if the size of the pigs vector is 1 when the user enters K, it will just kill off the 1 object
			}
			else
			{
			while(true)
			{
				int halfpig = pigs.size()/2;
				int dead;
				// declares two variables with one used to store the number of pig objects halved and another used to store a random number (index)
				dead=rand()%pigs.size();
				halfindex.push_back(dead);
				// stores dead with a random index from the pigs vector and then adds it to the halfindex vector 
				pigs.erase(pigs.begin()+dead);
				if(halfindex.size()==halfpig)
					break;
				// keeps on running the loop until the halfindex vector has the same size as the variable halfpig
				
			}
		
		   
			cout << "Half of the population has been killed." << endl;
			// tells the user half of the population has been killed 
			}
		}
	
        
		cout.rdbuf(outputFile.rdbuf());
		// causes any output to the console to be redirected to the file output.txt 
		years++;
		
		
		// increments years by 1 
		cout<<"----------------------------------------------------------------------"<<endl;
	}
		
	
	
    outputFile.close(); 
	// closes the txt file
	
	return 0;
}
