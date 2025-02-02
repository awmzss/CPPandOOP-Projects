#include <iostream>
using namespace std;

const int MAX_SPECIALIZATIONS = 20 + 1;
const int MAX_QUEUE = 5 + 1;

string names[MAX_SPECIALIZATIONS][MAX_QUEUE];
string status[MAX_SPECIALIZATIONS][MAX_QUEUE];
int positions[MAX_SPECIALIZATIONS];

void shift_right(int spec){
	for(int i = positions[spec]; i > 1; i--){
		names[spec][i] = names[spec][i-1];
		status[spec][i] = status[spec][i-1];
	}
}

void shift_left(int spec){
	for(int i = 1; i < positions[spec] - 1; i++){
		names[spec][i] = names[spec][i+1];
		status[spec][i] = status[spec][i+1];
	}
}

void add_patient(){
	int spec, stat;
	string name;
	
	cout << "Enter spec, name, status: ";
	cin >> spec >> name >> stat;
	
	if(spec < 1 || spec > 20){
		cout << "Sorry, this specialization is not avilable now\n";
	}else if(positions[spec] > 5){
		cout << "Sorry, we can't add more patients for this specialization now\n";
	}else if (stat >= 0 && stat <= 1){
		if(stat == 1){
			shift_right(spec);
			names[spec][1] = name;
			status[spec][1] = "Urgent";
		}else{
			names[spec][positions[spec]] = name;
			status[spec][positions[spec]] = "Regular";
		}
		positions[spec]++;
	}
}

void print_all(){
	for(int i = 1; i <= 20; i++){
		if(positions[i] > 1){
			cout << "**********\n";
			cout << "There are " << positions[i] - 1 << " patient/s in specialization " << i << '\n';
			for(int j = 1; j < positions[i]; j++){
				cout << names[i][j] << ' ' << status[i][j] << '\n';
			}
			cout << '\n';
		}
	}
}

void get_next(){
	int spec;
	
	cout << "Enter specialization: ";
	cin >> spec;
	
	if(spec < 1 || spec > 20){
		cout << "Invalid specialization\n";
	}else if(positions[spec] == 1){
		cout << "No patients at the moment. Have a rest, Dr.\n";	
	}else{
		cout << names[spec][1] << ", please go with the Dr.\n" << '\n';
		shift_left(spec);
		positions[spec]--;
	}
}

int menu(){
	int choice = -1;
	do{
		cout << "Enter your choice:\n";
		cout << "1) Add new patient\n";
		cout << "2) Print all patients\n";
		cout << "3) Get next patient\n";
		cout << "4) Exit\n";
		
		cin >> choice;
	}while(choice < 1 || choice > 4);
	
	return choice;
}

void hosbital_system(){
	while(true){
		int choice = menu();
				
		if(choice == 1){
			add_patient();
		}else if(choice == 2){
			print_all();
		}else if(choice == 3){
			get_next();
		}else{
			break;
		}
		
		cout << '\n';
	}
}

int main(){
	for(int i = 1; i <= 20; i++){
		positions[i] = 1;
	}
	
	hosbital_system();	
	return 0;
}
