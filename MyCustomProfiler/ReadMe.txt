Author: Krzysztof Balcerowski
========================================================================
    MyCustomProfiler  Overview
========================================================================
EXE_TO_PROFILE project is executable which is profiling

Example1_LoggingMethodEntry : Program retrievies messages from MSMQ and prints them to console

ProfilerLauncher : Launches EXE_TO_PROFILE and enables profiler

MyCustomProfiler - logic of profiler
 It defines in MyCustomProfilerImpl.h - > std::set<mdToken> MethodsToHook;// = { 0x06000002 };
 Set consist of method tokens which should be hooked by profiler.







/////////////////////////////////////////////////////////////////////////////
