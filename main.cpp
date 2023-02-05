#include <bits/stdc++.h>
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define MAX 10
#define esc 27
using namespace std;

char const* Path_Of_Root;
size_t Directory_Size = 2048;
char Directory_Current[2048];
int quit = 1;
static struct termios Setup;
static struct termios Store;
static int Inputed_Char = -1;
bool Check_File;

vector<dirent*> Directory_Files;
int Line_Stat = 0, Line_Inp = 0, Line_show = 0;
vector<string> Input;
vector<string> Backward, Forward;
int low = 0, high = low + MAX, cp = 1, Line_Mod = 0;
int Starting = 0, Ending = 0, ffind = 0;
FILE* Result;
struct winsize w;

int cmp(dirent* a, dirent* b) {            // to sort files according to name
  if (strcmp(a->d_name, b->d_name) < 0) {  // sorted in ascending order
    return 1;
  } else {
    return 0;
  }
  return 0;
}

void Motion(int x, int y) {  // to set the cursor position
  x = x + 2;
  cout << "\033[" << x << ";" << y << "H";
  fflush(stdout);
}

void Print_Dir() {  // printing all the files in the directory
  cout << "\033[H\033[J";
  struct stat filedata;

  unsigned int uid;
  unsigned int gid;

  struct group* g;
  struct passwd* pw;

  int x = low;
  int z = high;

  int y = Directory_Files.size(), t = Line_Mod;

  cout << "Permissions"
       << "\t"
       << ":"
       << "\t"
       << "File Size"
       << "\t"
       << ":"
       << "\t"
       << "User"
       << "\t"
       << "Group"
       << "\t\t"
       << ":"
       << "\t"
       << "Last Modified"
       << "\t\t"
       << ":"
       << "\t"
       << "File/Directory Name" << endl
       << endl;

  sort(Directory_Files.begin(), Directory_Files.end(),
       cmp);  // to sort the files

  int FILE_Iterator = x;
  while (FILE_Iterator < z && FILE_Iterator < y) {
    lstat(Directory_Files[FILE_Iterator]->d_name, &filedata);

    string per = "";
    per = per + ((S_ISDIR(filedata.st_mode)) ? "d" : "-");
    per = per + ((filedata.st_mode & S_IRUSR) ? "r" : "-");
    per = per + ((filedata.st_mode & S_IWUSR) ? "w" : "-");
    per = per + ((filedata.st_mode & S_IXUSR) ? "x" : "-");
    per = per + ((filedata.st_mode & S_IRGRP) ? "r" : "-");
    per = per + ((filedata.st_mode & S_IWGRP) ? "w" : "-");
    per = per + ((filedata.st_mode & S_IXGRP) ? "x" : "-");
    per = per + ((filedata.st_mode & S_IROTH) ? "r" : "-");
    per = per + ((filedata.st_mode & S_IWOTH) ? "w" : "-");

    // printf("\033[7;1;36m");

    // printf("\033[36m");
    printf("\033[0;31m");  // for providing the color to the data printed

    cout << per << "\t"
         << ":"
         << "\t";  // printing all the permissions

    double file_size = filedata.st_size;

    if (file_size < 100) {
      printf("%6.2fB ", file_size);  // printing the size in bytes

    }

    else {
      file_size = file_size / 1024;
      if (file_size < 100) {
        printf("%6.2fKB", file_size);  // printing the size in Kilo bytes

      } else {
        file_size = file_size / 1024;
        printf("%6.2fMB", file_size);  // printing the size in Mega bytes
      }
    }

    cout << "\t"
         << ":"
         << "\t";

    uid = geteuid();
    pw = getpwuid(uid);
    if (pw) {
      cout << pw->pw_name;
      cout << "\t" << pw->pw_name;  // printing user and group ownership
    }
    cout << "\t\t"
         << ":"
         << "\t";

    string datetime = ctime(&filedata.st_mtime);

    int s = datetime.size();
    for (unsigned int i = 0; i < s - 9; i++) {
      cout << datetime[i];  // printing the modified date
    }
    cout << "\t"
         << ":"
         << "\t";

    if (!S_ISDIR(
            filedata.st_mode)) {  // printing the name of file and directory
      cout << Directory_Files[FILE_Iterator]->d_name;
    } else if (S_ISDIR(filedata.st_mode)) {
      cout << Directory_Files[FILE_Iterator]->d_name;
    }

    cout << endl;
    FILE_Iterator++;
  }

  Motion(t - 10, 0);
  cout << "Normal Mode, Press : to switch for Command Mode";
  return;
}

void Path_Set(char const* path) {
  Path_Of_Root = path;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);  // to get the terminal width

  Line_Mod = w.ws_row - 5;
  Line_show = Line_Inp + 2;
  Line_Stat = Line_Mod + 2;
  Line_show = Line_Stat + 2;
  Line_Inp = Line_show - 2;
}

void Path_Get_Set(char const* Directory) {
  DIR* instance;
  struct dirent* Dirent_Entity;

  instance = opendir(Directory);  // open the directory
  if (instance == NULL) {
    cout << endl << "Unable to open the Directory!!" << endl;
    return;
  }

  chdir(Directory);

  getcwd(Directory_Current,
         Directory_Size);  // to get the current working directory
  // Directory_Current=get_current_dir_name();

  if (Directory_Files.empty() ==
      0) {  // clearing the files if vector is not empty
    Directory_Files.clear();
  }

  Dirent_Entity = readdir(instance);//reading all the elements one by one 
  while (Dirent_Entity != NULL) {
    Directory_Files.push_back(
        Dirent_Entity);  // pushing all the files of current directory
    Dirent_Entity = readdir(instance);
  }
  closedir(instance);  // close the directory

  int s = Directory_Files.size();
  int z;
  /*if(MAX>s){
  cp=s;
  }
  else{
  cp=MAX;
  }*/
  cp = 1;
  low = 0;
  if (MAX < s) {
    high = MAX;
  } else {
    high = s;
  }

  z = cp;
  Print_Dir();
  Motion(z, 0);
  return;
}

void Normal_Mode() {  // setting up the normal code
  tcgetattr(STDIN_FILENO, &Setup);

  Store = Setup;

  Store.c_lflag &= ~(ECHO | ICANON);

  Store.c_cc[VMIN] = 1;
  Store.c_cc[VTIME] = 0;

  if (tcsetattr(0, TCSANOW, &Store) != 0) {
    cout << endl << "Unable to change into the Normal Mode" << endl;
    return;
  }
}

void Denormal_Mode()  // setting up the denormal mode
{
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &Setup);
}

void Up_Scrolling() {
  if (cp - 1 > 0) {// if greater than zero then can move up 
    Motion(--cp, 0);
    return;
  }

  else if (low != 0) {
    --low;
    --high;
    Print_Dir();
    Motion(cp, 0);
    return;
  }
}
void Down_Scrolling() {
  int s = Directory_Files.size();
  if (cp < MAX && cp < s) {// if less than min(MAX,S) then can move down 
    Motion(++cp, 0);
    return;
  }

  else if (high != s) {
    ++low;
    ++high;
    Print_Dir();
    Motion(cp, 0);
    return;
  }
}
void Up_Going() {
  if (strcmp(Path_Of_Root, Directory_Current) != 0) {//push till rootpath is not equal dir current path
    Backward.push_back(string(Directory_Current));
    Path_Get_Set("../");
  }
}
void Go_Home() {
  if (strcmp(Path_Of_Root, Directory_Current) ==
      0) {  // don't do anything if current directory is home
    return;
  }

  if (strcmp(Path_Of_Root, Directory_Current) != 0) {
    Backward.push_back(string(Directory_Current));
    Path_Get_Set(Path_Of_Root);
  }
}
void Go_Back() {
  if (Backward.size() == 0) {  // return if backward is empty
    return;
  }

  if (Backward.size() != 0) {  // pop from backward if not empty
    string visit = Backward.back();
    Backward.pop_back();
    Forward.push_back(string(Directory_Current));
    Path_Get_Set(visit.c_str());
  }
}
void Forward_Going() {
  if (Forward.size() == 0) {  // return if forward is empty
    return;
  }

  if (Forward.size() != 0) {  // pop from forward if not empty
    string visit = Forward.back();
    Forward.pop_back();
    Backward.push_back(string(Directory_Current));
    Path_Get_Set(visit.c_str());
  }
}
void File_Open() {
  struct stat buffer;
  int Cursor_Position = cp + low - 1;
  char* File_Name = Directory_Files[Cursor_Position]->d_name;
  lstat(File_Name, &buffer);

  // printf("\033[36m");

  if (!S_ISDIR(buffer.st_mode)) {// if file then open using fork
    pid_t p = fork();

    if (p == 0) {
      execl("/usr/bin/xdg-open", "xdg-open", File_Name, NULL);
      exit(1);
    }
  }

  if (S_ISDIR(buffer.st_mode)) {  // if current directory then do nothing
    if (strcmp(File_Name, ".") == 0) {
      return;
    }

    if (strcmp(File_Name, "..") == 0 &&
        strcmp(Path_Of_Root, Directory_Current) ==
            0) {  // if preivous directory not exist then do nothing
      return;
    }

    Backward.push_back(string(Directory_Current));// push in backward so can come back 

    Path_Get_Set((string(Directory_Current) + '/' + string(File_Name)).c_str());
  }

  return;
}

void Clear_Cmd() {
  int i = Line_Mod, j = 0;
  int row = w.ws_row, column = w.ws_col;
  Motion(i, 0);

  for (; i < row; i++) {
    for (; j < column; j++) {
      cout << " ";
    }
  }
  Motion(i, 0);
}

void Split_Input(string in) {  // splitting the input enter using space in command code then
                               // store in vector
  Input.clear();
  int i = 0;
  string str = "";
  bool f = false;
  int s = in.size();
  for (i = 0; i < s; i++) {
    if (f) {
      f = false;
      continue;
    } else if (in[i] == 92) {
      str = str + " ";
      f = true;
    } else if (in[i] == ' ') {
      Input.push_back(str);
      str = "";
    }

    else {
      str.push_back(in[i]);
    }
  }
  s = str.size();
  if (s != 0) {
    Input.push_back(str);
  }
}

void File_Copying(string name, string destination) {
  char temp[1024];
  int infile = 0, outfile = 0, n = 0;
  infile = open(name.c_str(), O_RDONLY);// open the file in readonly mode 
  outfile = open((destination + '/' + name).c_str(), O_WRONLY | O_CREAT,
                 S_IRUSR | S_IWUSR);// creating the file at destination
  while ((n = read(infile, temp, sizeof(temp))) > 0) {
    write(outfile, temp, n);// write all the data of file at destination
  }
}

int Deleting_File(string path) {
  int ret;
  ret = remove(path.c_str());
  return ret;
}

void Directory_Copying(string dir, string destination) {
  struct stat buffer;
  DIR* dopen;
  struct dirent* entry;
  dopen = opendir(dir.c_str());  // opening the directory
  if (dopen == NULL) {
    cout << "Cannot open the directory: " << dir.c_str();
    return;
  }
  chdir(dir.c_str());
  entry = readdir(dopen);// reading the directory 
  while (entry != NULL) {// copying all the elements one by one 
    lstat(entry->d_name, &buffer);

    if (!S_ISDIR(buffer.st_mode)) {  // if file then directly copy
      File_Copying(entry->d_name, destination);
    }

    else if (S_ISDIR(buffer.st_mode) && strcmp(".", entry->d_name) != 0 &&
             strcmp("..", entry->d_name) != 0) {
      string str = destination + '/' + entry->d_name;

      mkdir(str.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);// make directory at the destination 

      Directory_Copying(entry->d_name, str);  // if folder then copy recursively
    }

    entry = readdir(dopen);  // reading all the files of directory
  }
  chdir("..");
  closedir(dopen);  // close the open directory
  return;
}

void Directory_Deleting(string dir) {
  DIR* dopen;
  struct dirent* entry;
  struct stat buffer;
  dopen = opendir(dir.c_str());  // open the dir directory
  if (dopen == NULL) {
    cout << "Cannot open the directory: " << dir.c_str();
    return;
  }
  chdir(dir.c_str());
  entry = readdir(dopen);
  while (entry != NULL) {
    lstat(entry->d_name, &buffer);

    if (!S_ISDIR(buffer.st_mode)) {  // if file then delete it
      unlink(entry->d_name);
    }

    else if (S_ISDIR(buffer.st_mode) && strcmp(".", entry->d_name) != 0 &&
             strcmp("..", entry->d_name) != 0) {
      Directory_Deleting(
          entry->d_name);  // if folder then delete folder recursively
      rmdir(entry->d_name);// remove the directory 
    }

    entry = readdir(dopen);
  }
  chdir(".."); // changing the directory to parent directory 
  closedir(dopen);// closing the directory 
}

void File_Searching(string File_Name, string dir) {
  DIR* dopen;
  struct dirent* entry;
  struct stat buffer;

  dopen = opendir(dir.c_str());  // open the dir directory
  if (dopen == NULL) {
    // cout << endl<<"Cannot open the directory: " << dir<<endl;
    return;
  }
  chdir(dir.c_str());
  entry = readdir(dopen);// reading the directory
  while (entry != NULL) {// loop over all the elements 
    lstat(entry->d_name, &buffer);
    if (S_ISDIR(buffer.st_mode) && strcmp(".", entry->d_name) != 0 &&
        strcmp("..", entry->d_name) != 0) {
      if (strcmp(entry->d_name, File_Name.c_str()) == 0) {  // directory found
        ffind = 1;

        return;

      } else {
        File_Searching(File_Name, entry->d_name);// recursively search in the folder 
      }
    } else if (strcmp(File_Name.c_str(), entry->d_name) == 0) {  // file found
      ffind = 1;

      return;
    }
    entry = readdir(dopen);
  }

  chdir(".."); //changed to parent directory 
  closedir(dopen);  // close the opened directory
}

void Creating() {
  string location;
  int s = Input.size();
  if (Input[s - 1][0] == '.') {// making the location path 
    location = Directory_Current;// same directory location
  } else if (Input[s - 1][0] == '/') {
    location = Directory_Current + Input[s - 1];
  } else if (Input[s - 1][0] == '~') {
    location = Path_Of_Root + Input[s - 1].substr(1);
  }

  else {
    Clear_Cmd();
    Motion(Line_Stat, 0);
    cout << endl << "Path is Invalid!" << endl;

    return;
  }
  int i = 0;
  for (i = 1; i <= s - 2; i++) {  // creating multiple files and directories
    string name = Input[i];
    if (Input[0] == "create_dir") {  // creating the directory
      mkdir((location + '/' + name).c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    } else if (Input[0] == "create_file") {  // creating the file
      open((location + '/' + name).c_str(), O_WRONLY | O_CREAT,
           S_IRUSR | S_IWUSR);
    }
  }
  Clear_Cmd();

  Motion(Line_Stat, 0);
  cout << endl << "Succesfully Created!" << endl;
}
void Moving_File() {
  string name, destination;
  int s = Input.size();
  if (Input[s - 1][0] == '/') {  // making the destination path
    destination = Directory_Current + Input[s - 1];
  } else if (Input[s - 1][0] == '~') {
    destination = Path_Of_Root + Input[s - 1].substr(1);
  }

  else {
    Clear_Cmd();
    Motion(Line_Stat, 0);
    cout << endl << "Path is Invalid!" << endl;

    return;
  }
  for (int i = 1; i <= s - 2; i++) {  // moving multiple files
    name = Input[i];
    File_Copying(name, destination);  //   copying the file into new position
    string str = Directory_Current + '/' + name;
    Deleting_File(str);  // deleting file from original path
  }

  Clear_Cmd();
  Motion(Line_Stat, 0);
  string str = Directory_Current + '/' + name;

  if (!Deleting_File(str)) {
    cout << endl << "Failed Deletion!" << endl;
  }
  Print_Dir();
  cout << endl << "Succesfully Moved!" << endl;
}
void Renaming() {
  string s1 = Input[1];
  string s2 = Input[2];
  rename(s1.c_str(), s2.c_str());  // files renamed
  Clear_Cmd();
  Motion(Line_Stat, 0);
  cout << endl << "Succesfully Renamed!" << endl;
}
void Moving_Directory() {
  string source, destination;
  int size = Input.size();
  if (Input[size - 1][0] == '~') {  // making the destination path
    destination = Path_Of_Root + Input[size - 1].substr(1);
  } else if (Input[size - 1][0] == '/') {
    destination = Directory_Current + Input[size - 1];
  } else {// location not possible 
    Clear_Cmd();
    Motion(Line_Stat, 0);
    cout << endl << "Invalid Path!" << endl;

    return;
  }
  for (int i = 1; i <= size - 2; i++) {
    source = Input[i];
    mkdir((destination + '/' + source).c_str(), S_IRUSR | S_IWUSR | S_IXUSR);// creating the directory
    Directory_Copying(Directory_Current + '/' + source,
                      destination + '/' + source);  // copying the directories
    Directory_Deleting(source);
    rmdir(source.c_str());// removing the directory 
  }
  Clear_Cmd();
  Motion(Line_Stat, 0);
  cout << endl << endl << endl << endl;
  cout << endl << "Succesfully Moved!" << endl;
}
void Mode_Toggling() {
  Clear_Cmd();
  cout << "Command Mode, Press ESC to go back to Normal Mode or quit to close "
          "the application "
       << endl;
  char c;
  int f = 1;
  string input = "";
  while (f == 1) {
    cout << endl << endl;
    Motion(Line_Stat, 0);
    cout << ": ";
    input = "";
    bool k = false;
    c = '$';
    while ((c = cin.get()) != esc && c != 10) {
      if (k && input.size() == 0) {
        cout << ": ";
      }

      cout << c;

      if (c == 127) {
        if (input.length() <= 1) {
          input = "";
          printf("\b \b");
          Motion(Line_Stat, 0);
        } else {
          input.pop_back();
          printf("\b\b");
        }
      } else {
        input = input + c;
      }
      k = true;
    }

    if (c == esc) {
      Path_Get_Set(Directory_Current);
      Motion(cp, 0);
      Denormal_Mode();
      break;
    } else {
      Split_Input(input);

      if (Input[0] == "search") {  // searching the file or directory
        string name = Input[1];

        File_Searching(name, Directory_Current);

        if (ffind == 1) {
          cout << endl << "True" << endl;
          ffind = 0;
        } else {
          cout << endl << "False" << endl;
          ffind = 0;
        }

      }

      else if (Input[0] == "move") {  // moving the file
        Moving_File();
      }

      else if (Input[0] == "move_dir") {  // moving the directory
        Moving_Directory();
      }

      else if (Input[0] == "delete_file") {  // deleting the file
        string path;
        if (Input[1][0] == '/') { // making the path to delete file 
          path = Directory_Current + Input[1];
        } else if (Input[1][0] == '~') {
          path = Path_Of_Root + Input[1].substr(1);
        }

        else {
          Clear_Cmd();
          Motion(Line_Stat, 0);
          cout << endl << "Path is Invalid!" << endl;
          continue;
        }
        Clear_Cmd();
        string str1 = Path_Of_Root + '/' + path;
        Motion(Line_Stat, 0);

        if (Deleting_File(str1)) {
          cout << endl << "Deletion is Failed!" << endl;
        }

      }

      else if (Input[0] == "delete_dir") {  // deleting the directory
        string destination;

        if (Input[1][0] == '/') {// making the path to delete directory 
          destination = Directory_Current + Input[1];
        } else if (Input[1][0] == '~') {
          destination = Path_Of_Root + Input[1].substr(1);
        }

        else {
          Clear_Cmd();
          Motion(Line_Stat, 0);
          cout << endl << " Path is Invalid!" << endl;
          continue;
        }
        Directory_Deleting(destination);
        rmdir(destination.c_str());// deleting the directory 
        Clear_Cmd();
        Motion(Line_Stat, 0);
        cout << endl << endl << endl << endl << endl;
        cout << "Succesfully Deleted!" << endl << endl;

      }

      else if (Input[0] == "create_file" || Input[0] == "create_dir") {
        Creating();  // creating file or directory

      }

      else if (Input[0] == "rename") {  // renaming the file or folder
        Renaming();
      }

      else if (Input[0] == "goto") {  // goto to the particular directory

        if ((Input[1][0] >= 'a' && Input[1][0] <= 'z') ||
            (Input[1][0] >= 'A' && Input[1][0] <= 'Z') ||
            (Input[1][0] >= '0' && Input[1][0] <= '9')) {
          Path_Get_Set((string(Directory_Current) + '/' + Input[1]).c_str());
        } else if (Input[1][0] == '~') {
          Path_Get_Set((Path_Of_Root + Input[1].substr(1)).c_str());

        }

        else if (Input[1][0] == '/') {
          Go_Home();
        } else if (Input[1][0] == '.' && Input[1][1] == '.') {// if parent directory 
          Path_Get_Set("..");
        } else if (Input[1][0] == '.') {// if same directory 
          Path_Get_Set(Directory_Current);
        } else {
          Path_Get_Set((Path_Of_Root + Input[1].substr(1)).c_str());
        }
        Motion(cp, 0);

      }

      else if (Input[0] == "copy_dir") {  // copy the directory
        string name, destination;
        int s = Input.size();
        int i = 1;
        if (Input[s - 1][0] == '/') {// making the destination path for copying 
          destination = Directory_Current + Input[s - 1];
        }

        else if (Input[s - 1][0] == '~') {
          destination = Path_Of_Root + Input[s - 1].substr(1);
        }

        else {
          Clear_Cmd();
          Motion(Line_Stat, 0);
          cout << endl << "Path is Invalid!" << endl;
          continue;
        }

        for (i = 1; i <= s - 2; i++) {
          name = Input[i];
          mkdir((destination + '/' + name).c_str(),
                S_IRUSR | S_IWUSR | S_IXUSR);// making directory
          Directory_Copying(Directory_Current + '/' + name,
                            destination + '/' + name);
        }
        Clear_Cmd();
        Motion(Line_Stat, 0);

      }

      else if (Input[0] == "copy") {  // copy the file
        string name, destination;
        int s = Input.size();
        int i = 1;
        if (Input[s - 1][0] == '/') {// making path to copy the file
          destination = Directory_Current + Input[s - 1];
        } else if (Input[s - 1][0] == '~') {
          destination = Path_Of_Root + Input[s - 1].substr(1);
        }

        else {
          Clear_Cmd();
          Motion(Line_Stat, 0);
          cout << endl << "Path is Invalid!" << endl;
          continue;
        }

        for (i = 1; i <= s - 2; i++) {  // copying multiple files
          name = Input[i];
          File_Copying(name, destination);
        }

        Clear_Cmd();
        Motion(Line_Stat, 0);

      } else if (Input[0] == "quit") {  // quiting from command mode
        quit = 0;

        f = 0;

      }

      else {  // invalid command entered
        Clear_Cmd();

        cout << endl;
        Motion(Line_Stat, 0);
        cout << endl << "Invalid Command!" << endl;
      }

      cout << endl << endl << endl;
    }
    if (f == 0) {// for quit 
      Path_Get_Set(Directory_Current);
      Motion(cp, 0);
      Denormal_Mode();
    }
  }

  Normal_Mode();
  return;
}

int main() {
  cout << "\033[H\033[J";
  char Current_Path[2048];
  size_t s = 2048;

  getcwd(Current_Path, s);  // getting the current working path

  Path_Set(Current_Path);  // setting the current path as root

  Path_Get_Set(Current_Path);
  cout << "\033[H\033[J";  // to clear the whole screen
  Path_Get_Set(Current_Path);

  Normal_Mode();  // switching to normal mode

  char c = '$';//default value

  while (c != 'q' && c != 'Q' && quit != 0) {
    c = cin.get();

    switch (c) {
      case 65:
        Up_Scrolling();
        break;

      case 66:
        Down_Scrolling();
        break;

      case 127:
        Up_Going();
        break;
      case 68:
        Go_Back();
        break;
      case 67:
        Forward_Going();
        break;
      case 'h':
        Go_Home();
        break;
      case 'H':
        Go_Home();
        break;
      case 10:
        File_Open();
        break;
      case ':':
        Mode_Toggling();
        break;
      default:
        break;
    }
  }

  Denormal_Mode();  // switching out of normal mode
  cout << "\033[H\033[J";//clearing the whole screen 

  exit(0);
}

