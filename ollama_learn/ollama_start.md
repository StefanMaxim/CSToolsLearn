Set up a llm server on your local computer. (NOTE: localhost means only works on that specific computer, so can ssh into it to set it up and run it there)
Ollama used for agentic AI
Write agent loop in python


Ollama is used to set up a local llm.
(Basically docker for language models + local API server)

Ollama run llama3
1: starts up model locally
2: spins up a server, usually https://localhost:12345
3:waits for requests


Sends requests to it like:

POST https://localhost:12345/api/generate, with 
Json:
{
“Model”: “llama3”,
“Prompt”: Hello”
}

And gets response
{‘response’: “Hi There”
“

So it goes agent -> localhost -> llama -> model

Or, can expose the IP via:

OLLAMA_HOST = 0.0.0.0 ollama serve (what this means: 0.0.0.0 means to listen on all ip addresses on the machine, and accepts connections from the outside)


Then, find your local ip address via:
Ifconfig or something

NOTE: the port number is actually fixed, and for ollama it is 11434 by default
The rest is just your local IP
OLLAMA_HOST=0.0.0.0, creates a temporary variable, which says that you can host it from any computer




## Starting Guide on Ollama

API = Application Programming Interface:
Essentially, it is rules for how one piece of software can interact with another
Tells:

1: what requests you can make
2: how to format them
3: what responses you can hope to get back

Different Types of APIs:

1:Library APIs: like printf() in c (since it is rules+function for c program to print to stdio)
2: OS APIs: stuff like system calls, that you make to your kernel
3: WEB API/ HTTPS API: send requests over https to another server

So, when ollama exposed an https api on your machine, usually at port https://localhost:11434
1: ollama runs a server process on your computer
2: it is listening on port 11434
3: other programs can send it https requests
4: ollama responds with data, usually in json format

This way, instead of the agent having to run "ollama serve; ollama run llama3"
it can just:
POST https://localhost:11434/api/generate followed by some json saying what model to use and what to do

CLI = command line interface, for humans to type
API = application programming interface, used so program and send data from one to another

What it does precisely:

11434: the port on your local computer
localhost: the ip placeholder, indicatign the current computer
https://localhost:11434, means send a https request to ollama server listening on that port

what about the /api/generate stuff?

REST API:
a REST API is an HTTPS API organized around standard HTTPS IDEAS:
GET to recieve data
POST to send data/trigger work
JSON request and response bodies
url paths for different resourses / actions
(essentially a REST API is an api that follows this formula, more loosely)

Thus, when people say
