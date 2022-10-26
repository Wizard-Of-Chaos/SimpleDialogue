# Simple Dialogue Tool
 A GUI tool for use in creating dialogue trees and exporting them to XML. Designed for use with other libraries.
 
 This tool was built with CMake, Qt 6.3 and about a day's worth of annoyance with menu design. It has since been updated with about another day's worth of annoyance with menu design and a few new features. In order to compile, compile this project with Qt however your conscience dictates and let fly.
 
 XML documents consist of a "tree", which contains "nodes" that contain choices. Nodes include a speaker, text of the node, and choices associated with that node. Choices in turn include their own text, the ID of the node they lead to (or "none" if there isn't a followup) and an effect, which by default can be "exit" or "none" (but of course you can add your own, although how your system handles said effects is up to you.
 
 UPDATE: The tool now supports removal of choices as well as setting flags, requiring flags and previous tree IDs, and removing the both of those as well as a quick re-design on the layout. Again, how you handle these flags is entirely up to you; my use case is global flags that get set and can be required for an individual choice to appear, as well as prior chunks of dialogue that might be required for a given choice (i.e., a choice only appears if you've already talked to the person about (x) topic before).
 
 Example XML format:
 
 ```xml
<?xml version="1.0" encoding="UTF-8"?>
<tree id="example_tree" minsector="0">
    <speakers>
        <speaker name="Steven Mitchell"/>
    </speakers>
    <node id="root">
        <speaker name="Steven Mitchell"/>
        <text text="This is a quick test to demonstrate the new dialogue tool."/>
        <choice text="Tell me more!" next="tellmemore" effect="none">
            <required_flag id="XML_REQUIREDFLAG_1"/>
            <required_tree id="XML_PRIORDIALOGUE_1"/>
            <sets_flag id="XML_FLAGSET_1"/>
        </choice>
        <choice text="I don't really care." next="none" effect="exit">
            <sets_flag id="XML_DONTCARE"/>
        </choice>
    </node>
    <node id="tellmemore">
        <speaker name="Steven Mitchell"/>
        <text text="Well, realistically, this new system allows for both effects and setting flags, which can then be messed with as you desire."/>
        <choice text="Run that by me again?" next="root" effect="none">
            <sets_flag id="XML_REPEATED"/>
            <sets_flag id="XML_USER_IDIOT"/>
        </choice>
        <choice text="Nifty. Thanks, Steven." next="none" effect="exit">
            <sets_flag id="XML_POLITE"/>
        </choice>
    </node>
</tree>

 ```
