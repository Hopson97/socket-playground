add_library(
    net
    event.cpp
    server.cpp
    client.cpp
)

#Set flags
if(MSVC)
  	target_compile_options(net PRIVATE 
    	/W4 /WX)
else()
  	target_compile_options(net PRIVATE 
		-Wall -Wextra -pedantic -Werror)		#Warning flags
endif()