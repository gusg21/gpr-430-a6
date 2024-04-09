#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <deque>

#include <socklib.h>
#include <defer.h>

#include <SDL.h>
#include <SDL_ttf.h>

void fatal(bool check, const char *msg)
{
  if (check)
  {
    perror(msg);
    exit(1);
  }
}

class MessageLog : public std::deque<std::string>
{
public:
  MessageLog(size_t maxRows = 20) : std::deque<std::string>(maxRows, "")
  {
  }

  void AddMessage(const std::string &message)
  {
    pop_front();
    push_back(message);
  }
};

void RenderText(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, const std::string &text, int x, int y)
{
  // Draw to surface and create a texture
  SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  // Setup the rect for the texture
  SDL_Rect dstRect;
  dstRect.x = x;
  dstRect.y = y;
  SDL_QueryTexture(texture, nullptr, nullptr, &dstRect.w, &dstRect.h);

  // Draw the texture
  SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

  // Destroy the surface/texture
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}

// Used for rendering messages
enum MessageSource
{
  USER,
  SERVER
};

// Not for student use. Adds a decoded character to the typing queue.
void pushch(char c);

// Gets the next character from the typing queue
// or returns EOF.
int getch();

bool is_backspace(char c) { return c == '\b'; }
bool is_newline(char c) { return c == '\n' || c == '\r'; }

// Adds a message to the log to be rendered.
void append_message(const std::string &msg, MessageSource src);

static MessageLog messageLog(8);

class MessageHolder
{
private:
  std::string message;
  Socket* sock;

public:
  MessageHolder()
  {
  }

  // Do whatever setup is necessary
  void Init()
  {
    // TODO: Implement this
    sock = new Socket(Socket::Family::INET, Socket::Type::STREAM);
    sock->Connect(Address("68.183.63.165", 24925));
    sock->SetNonBlockingMode(true);
  }

  // Used for rendering
  std::string GetCurrentMessage()
  {
    return message;
  }

  // Do all the work for a given frame
  void Update(float dt)
  {
    char c = getch();
    while (c != EOF)
    {
      if (is_backspace(c))
      {
        if (message.size() > 0)
          message.pop_back();
      }
      else if (is_newline(c))
      {
        // send
        sock->Send(message.c_str(), message.size());
        append_message(message, USER);
        message.clear();
      }
      else
      {
        message.push_back(c);
      }
      c = getch();
    }

    char buf[1024] = {0};
    sock->Recv(buf, 1024);
    if (0[buf] != '\0') { // tee hee
      append_message(std::string(buf), SERVER);
    }
  }
};

int RunProgram();
int RunTests();

int main(int argc, char *argv[])
{
  SockLibInit();
  atexit(SockLibShutdown);

  if (argc > 1)
  {
    return RunTests();
  }

  SDL_Init(SDL_INIT_EVERYTHING);
  if (TTF_Init() == -1)
  {
    std::cerr << "TTF failed to initialize\n";
  }

  TTF_Font *font = TTF_OpenFont("Carlito-Regular.ttf", 18);
  SDL_Window *window;
  SDL_Renderer *renderer;

  fatal(SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer) != 0, "CreateWindow");

  Uint64 last_frame = SDL_GetTicks();
  float target_dt = 1 / 60.f;

  MessageHolder messageHolder;
  messageHolder.Init();

  puts("Initialized.");

  bool quit = false;
  while (!quit)
  {
    Uint64 frame_start = SDL_GetTicks();
    float dt = (frame_start - last_frame) / 1000.f;
    if (dt < target_dt)
      continue;

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
      if (e.type == SDL_KEYDOWN)
      {
        if (e.key.keysym.sym == SDLK_ESCAPE)
        {
          quit = true;
        }

        unsigned char key = e.key.keysym.sym;
        // Printable characters
        if (e.key.keysym.mod & KMOD_SHIFT)
        {
          // capitalization
          if (key >= 97 && key <= 122)
            key -= 0x20;
          // symbols don't seem to have a nice mapping -- this only
          // works with QWERTY keyboards, though :/
          if (key == '1')
            key = '!';
          if (key == '2')
            key = '@';
          if (key == '3')
            key = '#';
          if (key == '4')
            key = '$';
          if (key == '5')
            key = '%';
          if (key == '6')
            key = '^';
          if (key == '7')
            key = '&';
          if (key == '8')
            key = '*';
          if (key == '9')
            key = '(';
          if (key == '0')
            key = ')';
          if (key == '`')
            key = '~';
          if (key == '-')
            key = '_';
          if (key == '=')
            key = '+';
          if (key == '/')
            key = '?';
          if (key == ';')
            key = ':';
        }
        if (key == '\b' || key == '\r' || key == '\n' || (key >= 32 && key <= 127))
          pushch(key);
      }
      else if (e.type == SDL_QUIT)
      {
        quit = true;
      }
    }

    messageHolder.Update(dt);

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_Color fg = {0, 0, 0, SDL_ALPHA_OPAQUE};
    std::string str = messageHolder.GetCurrentMessage();
    RenderText(renderer, font, fg, str, 10, 480 - 26);

    int penY = 0;
    for (const std::string &message : messageLog)
    {
      RenderText(renderer, font, fg, message.c_str(), 0, penY);
      penY += 18;
    }

    SDL_RenderPresent(renderer);

    last_frame = frame_start;
  }

  SDL_Quit();

  return 0;
}

int RunTests()
{
#define MSG messageHolder.GetCurrentMessage()
  MessageHolder messageHolder;
  float dt = 1 / 60.f;
  messageHolder.Init();

  pushch('a');
  pushch('b');
  pushch('c');
  if (!MSG.empty())
  {
    puts("FAIL: Characters prematurely added to messageHolder.");
    return 1;
  }

  messageHolder.Update(dt);
  if (MSG != "abc")
  {
    printf("FAIL: messageHolder contains %s but should have %s.\n", MSG.c_str(), "abc");
    return 1;
  }

  messageHolder.Update(dt);
  if (MSG != "abc")
  {
    printf("FAIL: Two consecutive calls to Update() changed message from \"abc\" to "
           "\"%s\". Message should stay the same if no new inputs occurred.\n",
           MSG.c_str());
    return 1;
  }

  pushch('\r');
  messageHolder.Update(dt);
  if (!MSG.empty())
  {
    puts("FAIL: 'enter' key did not clear message buffer");
    return 1;
  }

  // Wait a little bit for the message to get back to us
  SDL_Delay(100);
  messageHolder.Update(dt);

  int msgCount = 0;
  for (const std::string &str : messageLog)
  {
    if (!str.empty())
      msgCount++;
  }

  if (msgCount != 2)
  {
    printf("FAIL: %d messages in message queue, but should be 2\n", msgCount);
    return 1;
  }

  puts("All tests passed!");
  return 0;
#undef MSG
}

static std::deque<char> typing_queue;

void pushch(char c)
{
  typing_queue.push_back(c);
}

int getch()
{
  if (typing_queue.empty())
    return EOF;

  int c = typing_queue.front();
  typing_queue.pop_front();
  return c;
}

void append_message(const std::string &msg, MessageSource src)
{
  std::string prefix(src == USER ? "User: " : "Server: ");
  messageLog.AddMessage(prefix + msg);
}
