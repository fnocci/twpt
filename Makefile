##################################################################
#
# twpt -- the tw pthreads problem
#
# Copyright (C) 2010  Cheryl Fillekes
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
##################################################################


#########################################
# to build twpt binary, use 'make ptatm'
#
#  to invoke ptatm from cmd line, use:
#  usage: twpt -h (help)
#               -T <numThreads>
#               -X <Xfer amt>
#               -N <Num accounts>
#               -M <initial Max balance>
#               -q <min queue size>
#               -s <seed for rand>
#               -v verbose
#########################################

WFLAGS=  -Wall -Werror -Wwrite-strings -Wstrict-prototypes \
         -Wmissing-prototypes -Wmissing-format-attribute \
         -Wredundant-decls -Wformat-y2k -Wformat-security \
         -Wformat-nonliteral -Wundef -Wbad-function-cast \
         -Waggregate-return -Wmissing-declarations

objects = twpt_main.o \
          twpt_opts.o \
          twpt_init.o \
          twpt_threads.o 

CFLAGS=-g -std=c99  ${WFLAGS} -D_XOPEN_SOURCE=600

twpt: $(objects)
	gcc -std=c99 -g -o twpt $(objects) -lpthread

all: twpt

clean:
	rm -rf twpt $(objects)

