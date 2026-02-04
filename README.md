# Drones

CMake/C++ hobby project for development of software for custom-made drones. 

Using LLMs (Codex) integrated into my IDE (Jetbrains CLion) to explore the possibilities of AI while maintaining code quality and proper architecture (hardware abstractions for example).

So far it's been pretty good at solving build errors and generating board-specific code for me. I do have to steer it into specific directions every now and then when it decides that:
- Everything can just be in one file (multiple classes for example)
- Naming conventions from the middle-ages can be used instead of modern code style (eg m_* prefix for members)
- Drivers are suddenly generated specific to one HAL (instead of independent so it can be used on multiple different hardware boards)

However, aside from that it has increased the initial setup of my project. Getting a logger to run on serial port was quite fun to have an LLM do that for me. And letting it implement events/callbacks and specific hardware looks pretty good.

Have yet to test it though.
