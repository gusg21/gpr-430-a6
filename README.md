# Programming Assignment 6 -- Non-Blocking Sockets

The purpose of this assignment is to help you practice writing TCP
server code. You will create a server which will receive connections
from clients and sort numbers for them, like the server in the first
networking assignment did.

As always, to begin this assignment:
- Create a new repository from this template repository.
- Make your new repository private and owned by the CC-GPR-430
  organization.
- You can check to ensure that your assignment is working as expected
  by observing the workflow results on GitHub.
  - As always, you only need the tests to pass on one platform. It
    doesn't matter which.
- When you're ready to submit, simply submit a link to your
  repository.


## Assignment Details

You will write a "chat bot" that connects to a server at
IP address **68.183.63.165** and port **24925**.

Functions you should implement:
1. `MessageHolder::MessageHolder()` (optional)
2. `MessageHolder::Init()`
	- Set up the socket so it is ready to send and receive data each frame.
3. `MessageHolder::GetCurrentMessage()`
	- Returns the current (partial) message the user has typed.
4. `MessageHolder::Update(float dt)`
	- Performs the full frame update:
		- Uses `getch()` to process all typed character since the last
          update. See below for more information on using `getch()`.
			- Removes the last character from the current message if
              the retrieved character is a backspace. You can make use
              of `is_backspace(char c)` to check this.
			- Sends the message if a newline is retrieved (you can
              check this using `is_newline(char c)`). Retrieving a
              newline should also clear the current message and post
              the current message using `append_message(string,
              MessageSource)`.
		- Receives messages from the client, posting them with
          `append_message` if a message is received.
		  
Add whatever member variables in `MessageHolder` are necessary to have
your code work correctly.

Functions you should use:
1. `getch()` -- Returns a single character that the user has typed, or
   returns `EOF` if the queue of typed characters is empty.
2. `is_backspace()`, `is_newline()` -- return true if the given
   character is a backspace or a newline, respectively.
3. `append_message()` to post a complete message, tagging it as either
   a `USER` message if it came from keyboard input (i.e., `getch()`)
   or as a `SERVER` message if it came from the server.
			
## Grading
- Correctness: 30 points
- Submission and code style: 10 points

**Total: 40 points.**

As usual, grading will be done automatically when you submit to
github. However, the tests test only basic functionality; I will be
looking through your code to make sure things are behaving as
expected.

## Testing
You can run the tests on your code by running the `RunTests()`
function, which will attempt to use a `MessageHolder` to connect to
the server and 
