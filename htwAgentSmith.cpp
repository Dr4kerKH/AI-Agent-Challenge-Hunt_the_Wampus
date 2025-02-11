// YOUR NAME: ___
//
// CS 4318, spring 2024
// Agent Challenge 4: Hunt the wumpus
//
// Rename this file and the function below.  For example, if your agent name
// is Jones, rename this htwAgentSmith.cpp file to htwAgentJones.cpp and the
// htwAgentSmith function below to htwAgentJones.  Complete your agent
// function and turn it in on Blackboard.  Random-number generation is not
// allowed; your agent must be entirely deterministic.  Feel free to create
// other agents--each in a separate .cpp file--for comparison purposes, but
// turn in only one.  Test your agent(s) with
//
//    nice bash htwBuild.bash
//
// and then
//
//    nice ./htwRunSim
//
// Each submitted agent will explore each of many random 4x4 wumpus worlds
// to determine the standings, which will be posted soon after the agents
// are due.

#include "htw.h"

namespace
{
   // If you need to define any new types or functions, put them here in
   // this unnamed namespace.  But no variables allowed!

   struct KnowledgeBase
   {
      // Declare and initialize member variables here.
      // No dynamic memory allocation is allowed, but arrays are okay.
   };
}

// Rename and complete this agent function.
action htwAgentSmith(WumpusWorldSensor currentSensor, bool shouldReset)
{
   // Declare one static variable of type KnowledgeBase.
   static KnowledgeBase kb;
   // Only that one static variable is allowed!

   // Your function must end up returning a valid action.
   // No random-number generation allowed!

   if (shouldReset)
   {
      // Create a fresh knowledge base for the new wumpus world.
      kb = KnowledgeBase();
      // Just return without taking an action.
      return doNothing;
   }

   // Perform reasoning and return action here.
   return climbOut; // Replace this return statement.
}

/*

 - First, carefully comment your code above to clarify how it works.
 - Here, describe your approach and analyze it.  How exactly did you develop
   and test it?  What are its strengths and weaknesses?  Why do you expect
   it to do well against the other submitted agents?
 - Also make a note here if you talked about the assignment with anyone or
   gave or received any kind of help.

*/
