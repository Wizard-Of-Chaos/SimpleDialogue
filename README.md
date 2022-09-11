# Simple Dialogue Tool
 A GUI tool for use in creating dialogue trees and exporting them to XML. Designed for use with other libraries.
 
 This tool was built with CMake, Qt 6.3 and about a day's worth of annoyance with menu design. In order to compile, compile this project with Qt however your conscience dictates and let fly.
 
 XML documents consist of a "tree", which contains "nodes" that contain choices. Nodes include a speaker, text of the node, and choices associated with that node. Choices in turn include their own text, the ID of the node they lead to (or "none" if there isn't a followup) and an effect, which by default can be "exit" or "none" (but of course you can add your own, although how your system handles said effects is up to you.
 
 Example XML format:
 
 ```xml
 <?xml version="1.0" encoding="UTF-8"?>
<tree id="example_tree" minsector="0">
    <speakers>
        <speaker name="Martin Hally"/>
    </speakers>
    <node id="root">
        <speaker name="Martin Hally"/>
        <text text="Hi, everyone! I'm here to demonstrate the use of loading and saving XML documents with the GUI tool."/>
        <choice text="Neat." next="none" effect="exit"/>
        <choice text="I don't really care." next="none" effect="exit"/>
        <choice text="Tell me more." next="more_response" effect="none"/>
    </node>
    <node id="more_response">
        <speaker name="Martin Hally"/>
        <text text="Sure! The tool spits out and loads XML documents, with a tree that contains dialogue nodes and choices for each node. The choices carry an ID for the next node and an 'effect', which can be anything you want it to be."/>
        <choice text="I get it now. Thanks, Martin." next="none" effect="exit"/>
        <choice text="Can you explain all that again?" next="root" effect="none"/>
        <choice text="I'm never going to use this." next="none" effect="exit"/>
    </node>
</tree>
 ```
