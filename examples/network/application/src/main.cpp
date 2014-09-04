// main.cpp
// // "Network" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
//


// Just a wrapper to the pass the control to the main entry of paper3d engine.
int main(int argc, char* argv[])
{
    extern int pwin32main(int argc, char* argv[]);
    return pwin32main(argc, argv);
}
