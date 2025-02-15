#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
using namespace std;

int ToInt(string str){
	istringstream iss(str);
	int num;
	iss >> num;
	
	return num;
}

vector<string> SplitString(string s, string delimiter = ","){
	vector<string> substrs;
	
	int pos;
	string substr;
	while((pos = (int)s.find(delimiter)) != -1){
		substr = s.substr(0, pos);
		substrs.push_back(substr);
		s.erase(0, pos + delimiter.size());
	}
	substrs.push_back(s);
	return substrs;
}

vector<string> ReadFileLines(string path){
	vector<string> lines;
	
	fstream file_handler(path.c_str());
	if(file_handler.fail()){
		cout << "\n\nError: Can't open the file\n\n";
		return lines;
	}
	
	string line;
	while(getline(file_handler, line)){
		if(line.size() == 0)
			continue;
		lines.push_back(line);
	}
	
	file_handler.close();
	return lines;
}

int ReadInt(int low, int high){
	cout << "\nEnter number in range " << low << " - " << high << ": ";
	
	int value; cin >> value;
	if(low <= value && value <= high)
		return value;
	
	cout << "Error: Invalid number. Try again\n";
	return ReadInt(low, high);
}

int ShowReadMenu(vector<string> choices){
	cout << "\nMenu:\n";
	for(int ch = 0; ch < (int)choices.size(); ch++)
		cout << '\t' << ch+1 << ": " << choices[ch] << '\n';
	return ReadInt(1, choices.size());
}

void WriteFileLines(string path, vector<string> lines, bool append = true){
	auto status = ios::in | ios::out | ios::app;
	if(!append)
		status = ios::in | ios::out | ios::trunc;
		
	fstream file_handler(path.c_str(), status);
	if(file_handler.fail()){
		cout << "\n\nError: Can't open the file\n\n";
		return;
	}
	
	for(auto &line: lines)
		file_handler << line << '\n';
	file_handler.close();
}

struct Question{
	int question_id, parent_question_id;
	int from_user_id, to_user_id;
	bool is_anonymous_question;
	string question_text, answer_text;
	
	Question(){
		question_id = parent_question_id = from_user_id = to_user_id = -1;
		is_anonymous_question = 1;
	}
	
	Question(string line){
		vector<string> substrs = SplitString(line);
		
		question_id = ToInt(substrs[0]);
		parent_question_id = ToInt(substrs[1]);
		from_user_id = ToInt(substrs[2]);
		to_user_id = ToInt(substrs[3]);
		is_anonymous_question = ToInt(substrs[4]);
		question_text = substrs[5];
		answer_text = substrs[6];
	}
	
	void PrintToQuestion(){
		string prefix = "";
		if(parent_question_id != -1)
			prefix = "\tThread: ";
		
		cout << prefix << "Question id(" << question_id << ") ";
		
		if(!is_anonymous_question)
			cout << "from user id(" << from_user_id << ") ";
			
		cout << '\t' << "Question: " << question_text << '\n';
		
		if(answer_text != "")
			cout << "\tAnswer: " << answer_text << '\n';
		cout << '\n';
	}
	
	void PrintFromQuestion(){
		cout << "Question id(" << question_id << ") ";
		if(!is_anonymous_question)
			cout << "!AQ";
		
		cout << "to user id(" << to_user_id << ") ";
		cout << "\t Question: " << question_text;
		
		if(answer_text != "")
			cout << "\t Answer: " << answer_text << '\n';
		else
			cout << "\t NOT ANSWERED YET\n";
	}
	
	string ToString(){
		ostringstream oss;
		oss << question_id << "," << parent_question_id << "," << from_user_id << "," << to_user_id << "," << is_anonymous_question << "," << question_text << "," << answer_text << '\n';
		return oss.str();
	}
	
	void PrintFeedQuestion(){
		if(parent_question_id != -1)
			cout << "Thread parent question id(" << parent_question_id << ") ";
		
		cout << "Question id(" << question_id << ") ";
		
		if(!is_anonymous_question)
			cout << "From user id(" << from_user_id << ") ";
		cout << "To user id(" << to_user_id << ") ";
		
		cout << "\tQuestion: " << question_text << '\n';
		if(answer_text != "")
			cout << "\tAnswer: " << answer_text << '\n';
	}
};

struct User{
	int user_id;
	string username, password, name, email;
	bool allow_anonymous_questions;
	
	vector<int> questions_id_from_me;
	map<int, vector<int>> questionid_questionidsThread;
	
	User(){
		user_id = allow_anonymous_questions = -1;
	}
	
	User(string line){
		vector<string> substrs = SplitString(line);
		
		user_id = ToInt(substrs[0]);
		username = substrs[1];
		password = substrs[2];
		name = substrs[3];
		email = substrs[4];
		allow_anonymous_questions = ToInt(substrs[5]);
	}
	
	string ToString(){
		ostringstream oss;
		oss << user_id << "," << username << "," << password << "," << name << "," << email << "," << allow_anonymous_questions;
		return oss.str();
	}
};

struct QuestionsManager{
	int last_id;
	map<int, Question> questionid_questionobject_map;
	map<int, vector<int>> questionid_questionidsThread_to_map;
	
	QuestionsManager(){
		last_id = 0;
	}
	
	void LoadDatabase(){
		last_id = 0;
		questionid_questionobject_map.clear();
		questionid_questionidsThread_to_map.clear();
		
		vector<string> lines = ReadFileLines("questions.txt");
		for(auto &line: lines){
			Question question(line);
			last_id = max(last_id, question.question_id);
			questionid_questionobject_map[question.question_id] = question;
			
			if(question.parent_question_id == -1)
				questionid_questionidsThread_to_map[question.question_id].push_back(question.question_id);
			else
				questionid_questionidsThread_to_map[question.parent_question_id].push_back(question.question_id);
		}
	}
	
	void FillUserQuestions(User &user){
		user.questions_id_from_me.clear();
		user.questionid_questionidsThread.clear();
		
		for(auto &pair: questionid_questionidsThread_to_map){
			for(auto &question_id: pair.second){
				Question question = questionid_questionobject_map[question_id];
				
				if(question.from_user_id == user.user_id)
					user.questions_id_from_me.push_back(question.question_id);
				
				if(question.to_user_id == user.user_id){
					if(question.parent_question_id == -1)
						user.questionid_questionidsThread[question.question_id].push_back(question.question_id);
					else
						user.questionid_questionidsThread[question.parent_question_id].push_back(question.question_id);
				}
			}
		}
	}
	
	void PrintQuestionsToUser(User &user){
		cout << '\n';
		
		if(user.questionid_questionidsThread.empty())
			cout << "No Questions";
			
		for(auto &pair: questionid_questionidsThread_to_map){
			for(auto question_id: pair.second){
				Question &question = questionid_questionobject_map[question_id];
				
				if(question.to_user_id == user.user_id)
					question.PrintToQuestion();
			}
		}
		cout << '\n';
	}
	
	void PrintQuestionsFromUser(User &user){
		cout << '\n';
		if(user.questions_id_from_me.empty())
			cout << "NO questions";
		
		for(auto &question_id: user.questions_id_from_me){
			Question &question = questionid_questionobject_map[question_id];
			question.PrintFromQuestion();
		}
		cout << '\n';
	}
	
	int ReadQuestionIdAny(User &user){
		cout << "Enter question id or -1 to cancel: ";
		int question_id; cin >> question_id;
		
		if(question_id == -1)
			return question_id;
		
		if(!questionid_questionobject_map.count(question_id)){
			cout << "Error: No question with such id. Try again\n\n";
			return ReadQuestionIdAny(user);
		}
		
		Question &question = questionid_questionobject_map[question_id];
		if(question.to_user_id != user.user_id){
			cout << "Error: Question is not for you. Try again\n\n";
			return ReadQuestionIdAny(user);
		}
		
		return question_id;
	}
	
	void AnswerQuestion(User &user){
		int question_id = ReadQuestionIdAny(user);
		if(question_id == -1)
			return;
		
		Question &question = questionid_questionobject_map[question_id];
		
		question.PrintToQuestion();
		if(question.answer_text != "")
			cout << "\nWarning: Already answered. Answer will be updated\n";
		
		cout << "Enter answer: ";
		getline(cin, question.answer_text);
		getline(cin, question.answer_text);
	}
	
	void DeleteQuestion(User &user){
		int question_id = ReadQuestionIdAny(user);
		if (question_id == -1)
			return;
		
		vector<int> ids_to_remove;
		if(questionid_questionidsThread_to_map.count(question_id)){ 
			ids_to_remove = questionid_questionidsThread_to_map[question_id];
			questionid_questionidsThread_to_map.erase(question_id);
		}else{
			ids_to_remove.push_back(question_id);
			
			for(auto &pair: questionid_questionidsThread_to_map){
				vector<int> &vec = pair.second;
				for(int i = 0; i < (int)vec.size(); i++){
					if(question_id == vec[i]){
						vec.erase(vec.begin() + i);
						break;
					}
				}
			}
		}
		
		for(int &id: ids_to_remove)
			questionid_questionobject_map.erase(id);
	}
	
	void UpdateDatabase(){
		vector<string> lines;
		for(auto &pair: questionid_questionobject_map)
			lines.push_back(pair.second.ToString());
		WriteFileLines("questions.txt", lines, false);
	}
	
	int ReadQuestionIdThread(){
		cout << "For thread question: Enter question id. Or -1 for new question: ";
		int question_id; cin >> question_id;
		
		if(question_id == -1)
			return question_id;
			
		if(!questionid_questionobject_map.count(question_id)){
			cout << "No thread question with such ID. Try again\n";
			return ReadQuestionIdThread();
		}
		
		return question_id;
	}
	
	void AskQuestion(User &user, pair<int, int> to_user_pair){
		Question question;
		
		if(to_user_pair.second == -1){
			cout << "Note: Anonymous questions are not allowed for this user\n";
			question.is_anonymous_question = 0;
		}else{
			cout << "Is anonymous questions?: (0 or 1): ";
			question.is_anonymous_question = ReadInt(0, 1);
		}
		
		question.parent_question_id = ReadQuestionIdThread();
		
		cout << "Enter question text: ";
		getline(cin, question.question_text);
		getline(cin, question.question_text);
		
		question.from_user_id = user.user_id;
		question.to_user_id = to_user_pair.first;
		
		question.question_id = ++last_id;
		questionid_questionobject_map[last_id] = question;
		
		if(question.parent_question_id == -1)
			questionid_questionidsThread_to_map[question.question_id].push_back(question.question_id);
		else
			questionid_questionidsThread_to_map[question.parent_question_id].push_back(question.question_id);		
	}
	
	void ListFeed(){
		for(auto &pair: questionid_questionobject_map){
			Question &question = pair.second;
			
			if(question.answer_text == "")
				continue;
			question.PrintFeedQuestion();
		}
	}
};

struct UsersManager{
	map<string, User> username_userobject_map;
	User current_user;
	int last_id;
	
	UsersManager(){
		last_id = 0;
	}
	
	void LoadDatabase(){
		last_id = 0;
		username_userobject_map.clear();
		
		vector<string> lines = ReadFileLines("users.txt");
		for(auto &line: lines){
			User user(line);
			username_userobject_map[user.username] = user;
			last_id = max(last_id, user.user_id);
		}		
	}
	
	void AccessSystem(){
		int choice = ShowReadMenu({"Login", "Sign Up"});
		if(choice == 1)
			DoLogin();
		else
			DoSignUp();
	}
	
	void DoLogin(){
		LoadDatabase();
		
		while(true){
			cout << "Enter username & password: ";
			cin >> current_user.username >> current_user.password;
			
			if(!username_userobject_map.count(current_user.username)){
				cout << "\nInvalid username or password. Try again\n\n";
				continue;
			}
			
			User user_exist = username_userobject_map[current_user.username];
			if(user_exist.password != current_user.password){
				cout << "\nInvalid username or password. Try again\n\n";
				continue;
			}
			
			current_user = user_exist;
			break;
		}
	}
	
	void DoSignUp(){
		while(true){
			cout << "Enter username(No spaces): ";
			cin >> current_user.username;
			
			if(username_userobject_map.count(current_user.username))
				cout << "Already user. Try again\n";
			else
				break;
		}
		
		cout << "Enter password: ";
		cin >> current_user.password;
		
		cout << "Enter name: ";
		cin >> current_user.name;
		
		cout << "Enter email: ";
		cin >> current_user.email;
		
		cout << "Allow anonymous questions? (0 or 1): ";
		cin >> current_user.allow_anonymous_questions;
		
		current_user.user_id = ++last_id;
		username_userobject_map[current_user.username] = current_user;
		UpdateDatabase(current_user);
	}
	
	void UpdateDatabase(User &user){
		string line = user.ToString();
		vector<string> lines(1, line);
		WriteFileLines("users.txt", lines);
	}
	
	pair<int, int> ReadUserId(){
		cout << "Enter user id or -1 to cancel: ";
		int user_id; cin >> user_id;
		
		if(user_id == -1)
			return {-1, -1};
		
		for(auto &pair: username_userobject_map){
			if(pair.second.user_id == user_id)
				return {user_id, pair.second.allow_anonymous_questions};
		}
		
		cout << "Invalid user id. Try Again\n";
		return ReadUserId();
	}
	
	void ListUsersNamesIds(){
		for(auto &pair: username_userobject_map)
			cout << "Id: " << pair.second.user_id << "\t\tName: " << pair.second.name << '\n';
	}
};

struct AskMeSystem{
	UsersManager users_manager;
	QuestionsManager questions_manager;
	
	void LoadDatabase(bool fill_user_questions = false){
		users_manager.LoadDatabase();
		questions_manager.LoadDatabase();
		
		if(fill_user_questions)
			questions_manager.FillUserQuestions(users_manager.current_user);
	}
	
	void run(){
		LoadDatabase();
		users_manager.AccessSystem();
		questions_manager.FillUserQuestions(users_manager.current_user);
				
		vector<string> menu;
		menu.push_back("Print Questions To Me");
		menu.push_back("Print Questions From Me");
		menu.push_back("Answer Question");
		menu.push_back("Delete Question");
		menu.push_back("Ask Question");
		menu.push_back("List System Users");
		menu.push_back("Feed");
		menu.push_back("Logout");
		
		while(true){
			int choice = ShowReadMenu(menu);
			LoadDatabase(true);
			
			if(choice == 1)
				questions_manager.PrintQuestionsToUser(users_manager.current_user);
			else if(choice == 2)
				questions_manager.PrintQuestionsFromUser(users_manager.current_user);
			else if(choice == 3){
				questions_manager.AnswerQuestion(users_manager.current_user);
				questions_manager.UpdateDatabase();
			}else if(choice == 4){
				questions_manager.DeleteQuestion(users_manager.current_user);
				questions_manager.FillUserQuestions(users_manager.current_user);
				questions_manager.UpdateDatabase();
			}else if(choice == 5){
				pair<int, int> to_user_pair = users_manager.ReadUserId();
				if(to_user_pair.first != -1){
					questions_manager.AskQuestion(users_manager.current_user, to_user_pair);
					questions_manager.UpdateDatabase();
				}
			}else if(choice == 6)
				users_manager.ListUsersNamesIds();
			else if(choice == 7)
				questions_manager.ListFeed();
			else
				break;
		}
		run();
	}
};

int main(){
	AskMeSystem service;
	service.run();
	
	return 0;
}
