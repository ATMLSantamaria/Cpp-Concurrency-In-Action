// DAEMON THREADS
// Calling detach on a std::thread object leave the thread to run in the background

// We dont have any mean to communicate with it, and we cannot get a std::thread object that references it.

// The ownership is passed over to the C++ RunTime Library, which ensures that the resources associated
// are reclaimed when the thread exits.

// This threads are called daemon threads, aka the UNIX concept of 
// daemon process (run in the background without any user interface)

// One example of a application of detached threads is a word processor (aka Microsoft Word)
// where you may edit many documents at once. So multiple, independent, top-level windows, one per document

// Each window is completely independent but runs in the same application

// Each thread run the same code but with different data related with its corresponding document.



// Outline example detaching a thread to handle other documents
#include <string>
#include <thread>

void openDocumentAndDisplayGui(std::string filename) {}
bool doneEditing(){return false;}
struct UserCommand {
  enum CommandType {closeDocument, openNewDocument};
  CommandType type;
};
UserCommand getUserInput();

const std::string getFilenameFromUser(); 

void processUserInput(UserCommand command);

void editDocument(std::string const& filename) {

  openDocumentAndDisplayGui(filename);
  while (!doneEditing()) {
    UserCommand cmd = getUserInput();
    UserCommand getUserInput();
    if (cmd.type == UserCommand::openNewDocument) {
      std::string const newName = getFilenameFromUser();
      std::thread t(editDocument,newName);
      t.detach();
    } else {
      processUserInput(cmd);
    }
  }

}