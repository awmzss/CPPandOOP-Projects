#include <iostream>
#include <algorithm>
using namespace std;

const int MAX_BOOKS = 10;
const int MAX_USERS = 10;

struct user{
	int id, len, borrowed_books[MAX_BOOKS];
	string name;
		
	user(){
		name = "",
		len = 0,
		id = -1;
	}
	
	void read(){
		cout << "Enter user name & national id: ";
		cin >> name >> id;
	}
	
	void borrow(int book_id){
		borrowed_books[len++] = book_id;
	}
	
	int return_copy(int book_id){
		bool removed = false;
		for(int i = 0; i < len; i++){
			if(borrowed_books[i] == book_id){
				removed = true;
				for(int j = i; j < len; j++){
					borrowed_books[j] = borrowed_books[j+1];
				}
				len--;
				break;
			}
		}
		
		if(!removed){
			cout << "User " << name << 
				" never borrowed book id " << book_id << '\n';
			return 0;
		}
		return 1;
	}
	
	void print(){
		sort(borrowed_books, borrowed_books+len);
		
		cout << "User " << name << " id " << id << 
			" borrowed books ids: ";
		for(int i = 0; i < len; i++)
			cout << borrowed_books[i] << ' ';
		cout << '\n';
	}
};

struct book{
	int id, total_quantity, total_borrowed, list[MAX_USERS];
	string name;
	
	book(){
		id = -1,
		total_quantity =
		total_borrowed = 0,
		name = "";
	}
	
	void read(){
		cout << "Enter book id & name & quantity: ";
		cin >> id >> name >> total_quantity;
	}
	
	bool has_prefix(string prefix){
		if(name.size() < prefix.size())
			return false;
		
		for(int i = 0; i < (int)prefix.size(); i++){
			if(name[i] != prefix[i])
				return false;
		}
		return true;
	}
	
	void print(){
		cout << "id: " << id << ", name: " << name << ", total_quantity: "
				<< total_quantity << ", total_borrowed: " << total_borrowed
				<< "\n";
	}
	
	void borrow(int user_idx){
		list[total_borrowed++] = user_idx;
	}
	
	void return_copy(int user_idx){
		for(int i = 0; i < total_borrowed; i++){
			if(list[i] == user_idx){
				for(int j  = i; j < total_borrowed; j++)
					list[j] = list[j+1];
				total_borrowed--;
				break;
			}
		}
	}
};

bool cmp_book_by_id(book &a, book &b){
	return a.id < b.id;
}

bool cmp_book_by_name(book &a, book &b){
	return a.name < b.name;
}

struct library_system{
	int total_books;
	book books[MAX_BOOKS];
	int total_users;
	user users[MAX_USERS];
	
	library_system(){
		total_books = total_users = 0;
	}
	
	void run(){
		while(true){
			int choice = menu();
			
			if (choice == 1)
				add_book();
			else if (choice == 2)
				search_books_by_prefix();
			else if (choice == 3)
				print_who_borrowed_book_by_name();
			else if (choice == 4)
				print_library_by_id();
			else if (choice == 5)
				print_library_by_name();
			else if (choice == 6)
				add_user();
			else if (choice == 7)
				user_borrow_book();
			else if (choice == 8)
				user_return_book();
			else if (choice == 9)
				print_users();
			else
				break;	
		}
	}
	
	int menu(){
		int choice = -1;	
		
		do{
			cout << "\n\nLibrary Menu;\n";
			cout << "1) add_book\n";
			cout << "2) search_books_by_prefix\n";
			cout << "3) print_who_borrowed_book_by_name\n";
			cout << "4) print_library_by_id\n";
			cout << "5) print_library_by_name\n";
			cout << "6) add_user\n";
			cout << "7) user_borrow_book\n";
			cout << "8) user_return_book\n";
			cout << "9) print_users\n";
			cout << "10) Exit\n";
			
			cin >> choice;
		}while(choice < 1 || choice > 10);
		
		return choice;
	}
	
	void add_book(){
		books[total_books++].read();
	}
	
	void search_books_by_prefix(){
		cout << "Enter book name prefix: ";
		string prefix; cin >> prefix;
		cout << '\n';
		
		int cnt = 0;
		for(int i = 0; i < total_books; i++){
			if(books[i].has_prefix(prefix)){
				cout << books[i].name << '\n';
				cnt++;
			}
		}
		
		if(!cnt)
			cout << "No books with such prefix\n";
	}
	
	int find_book_idx_by_name(string book_name){
		for(int i = 0; i < total_books; i++){
			if(books[i].name == book_name)
				return i;
		}
		return -1;
	}
	
	void print_who_borrowed_book_by_name(){
		cout << "Enter book name: ";
		string name; cin >> name;
		
		int book_idx = find_book_idx_by_name(name);
		if(book_idx == -1){
			cout << "No book with such name\n";
			return;
		}
		
		if(!books[book_idx].total_borrowed){
			cout << "No borrowed copies\n";
			return;
		}
		
		for(int i = 0; i < books[book_idx].total_borrowed; i++){
			cout << users[books[book_idx].list[i]].name << '\n';
		}
	}
	
	void print(){
		cout << '\n';
		for(int i = 0; i < total_books; i++){
			books[i].print();
		}
	}
	
	void print_library_by_id(){
		sort(books, books+total_books, cmp_book_by_id);
		print();
	}
	
	void print_library_by_name(){
		sort(books, books+total_books, cmp_book_by_name);
		print();
	}
	
	void add_user(){
		users[total_users++].read();
	}
	
	int find_user_idx_by_name(string user_name){
		for(int i = 0; i < total_users; i++){
			if(users[i].name == user_name)
				return i;
		}
		return -1;
	}
	
	int read_user(){
		string name; cin >> name;
		return find_user_idx_by_name(name);
	}
	
	int read_book(){
		string name; cin >> name;
		return find_book_idx_by_name(name);
	}
	
	int read_user_name_and_book_name(int &user_idx, int &book_idx){
		cout << "Enter user name & book name: ";
		user_idx = read_user();
		book_idx = read_book();
		
		if(user_idx == -1){
			cout << "No user with such index\n";
			return 0;
		}
		if(book_idx == -1){
			cout << "No book with such index\n";
			return 0;
		}
		
		return 1;
	}
	
	void user_borrow_book(){
		int user_idx, book_idx;
		
		if(!read_user_name_and_book_name(user_idx, book_idx))
			return;
		
		if(books[book_idx].total_borrowed == 
			books[book_idx].total_quantity){
				cout << "No copies available now for the book\n";
				return;
		}
		
		users[user_idx].borrow(books[book_idx].id);
		books[book_idx].borrow(user_idx);
	}
	
	void user_return_book(){
		int user_idx, book_idx;
		
		if(!read_user_name_and_book_name(user_idx, book_idx))
			return;
			
		if(users[user_idx].return_copy(books[book_idx].id))
			books[book_idx].return_copy(user_idx);
	}
	
	void print_users(){
		for(int i = 0; i < total_users; i++)
			users[i].print();
	}
};

int main(){
	library_system library;
	
	library.run();
	
	return 0;
}
