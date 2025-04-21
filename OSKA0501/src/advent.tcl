#!/usr/local/bin/tclsh
# Copyright Johann Oskarsson <myrkraverk@users.sourceforge.net>
package require tclreadline
set venture [ open "|./advent -prompt" RDWR ]
set line ""
while { $line != "END:" } {
    gets $venture line
    if { $line == "QUESTION:" } {
#	puts -nonewline stdout "yes/no?> "
	flush stdout
#	gets stdin answer
	set answer [ ::tclreadline::readline read "yes/no?> " ]
	puts $venture $answer
	flush $venture
	continue
    }
    if { $line == "INPUT:" } {
#	puts -nonewline stdout ">>--:> "
	flush stdout
#	gets stdin input
	set input [ ::tclreadline::readline read ">>--:> " ]
	puts $venture $input
	flush $venture
	continue
    }
    if { $line == "END:" } { # a hack to get rid of "END:" at termination
	continue
    }
    puts $line
}
