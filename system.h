/* This file is part of ukbdc.
 *
 * ukbdc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ukbdc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ukbdc; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*! \defgroup SYSTEM
 * \brief nonpreemptive multitasking and publisher-subscriber messaging system
 *
 * The module SYSTEM implements a simple nonpreemptive multitasking
 * capability. Every task is represented by a callback (\ref task_callback_t)
 * and priority. Tasks are run in cycles, and the `priority` parameter
 * determines how frequently a task is run. The lower the value of `priority`
 * the more frequently the task is run.
 *
 * Except for multitasking, this module also implements a simple
 * publisher-subscriber messaging system. The publisher can choose one of the
 * message types defined in system_messages.h and also one of 255 subtypes and
 * include a pointer to any data understood by the subscriber.
 * The subscriber can subscribe to one message type and choose either to
 * subscribe to only one of its subtypes or to all of them.
 *
 * Example
 * -------
 *  Suppose we are writing a module MYMODULE which will consist of a single
 *  task, which will be run periodically.
 *
 *  Assume a message type `MY_MESSAGE` has been created in system_messages.h.
 *  Assume this message will be published each time something important
 *  happens in MYMODULE. The message type will be set to a number which
 *  represents what has happened.
 *
 *  ### Adding tasks and publishing messages
 *
 *  The initialization function of MYMODULE can look like this:
 *
 *  	void MYMODULE_init()
 *  	{
 *  		SYSTEM_add_task(&MYMODULE_maintask, 5);
 *  	}
 *
 *  The function `MYMODULE_maintask` can be implemented as follows:
 *
 *  	void MYMODULE_maintask()
 *  	{
 *  		if (something_important_happens())
 *  			SYSTEM_publish_message(MY_MESSAGE, 0, NULL);
 *		else if (someotherthing_happens())
 *			SYSTEM_publish_message(MY_MESSAGE, 1, NULL);
 *		do_some_other_things();
 *  	}
 *
 *  In the program's `main` function, after calling `MYMODULE_init()` and
 *  starting the main loop with \ref SYSTEM_main_loop(), MYMODULE's task will be
 *  run every 6th cycle and it will generate 2 subtypes of `MY_MESSAGE` message.
 *
 *  ### Subscribing to messages
 *
 *  In order to react to the messages published by MYMODULE, one can write a
 *  message handler and subscribe to any message from any place in the
 *  program. In order to subscribe to one particular subtype, use:
 *
 *  	SYSTEM_subscribe(MY_MESSAGE, 1, &my_message_handler);
 *
 *  It is also possible to subscribe to any subtype in which case the handler
 *  will be called no matter which subtype of message was generated:
 *
 *  	SYSTEM_subscribe(MY_MESSAGE, ANY, &my_message_handler);
 *
 *  Where `my_message_handler` is a function of type \ref message_callback_t
 *  and can look like this:
 *
 *  	void my_message_handler(void *data)
 *  	{
 *  		do_something();
 *  	}
 *
 *  The function is passed one argument, a pointer to arbitrary data. The
 *  value of this pointer depends on the publisher and can be used to pass any
 *  extra data to the subscriber. In this example the publisher simply passes
 *  a null pointer.
 *
 *  @{
 */

#pragma once

#include <stdint.h>
#include "list.h"

/*! Error returned on subscription or publishing of a message type which does not
 * exist */
#define ERR_WRONG_TYPE -1
/*! Used to subscribe to all message subtypes */
#define ANY 0xff

/*! Type of function which implements a task */
typedef void (*task_callback_t)();

/*! A structure representing a single task */
typedef struct {
	/*! `0` means run every cycle, `1` - every second cycle, etc */
	int priority;
	/*! Cycle in which the task was last run */
	int last_run;
	/*! The function to call as callback */
	task_callback_t callback;
} task_t;

#include "system_messages.h"

/*! Type of function which handles a message */
typedef void (*message_callback_t)(void *data);

/*! Message handler consists of a callback which is run whenever a message of
 * particular type is published and a \ref subtype which determines whether
 * the handler handles only one or all subtypes of message. */
typedef struct {
	/*! subtype number of \ref ANY if the handler should handle all
	 * subtypes */
	uint8_t subtype;
	/*! handlers' callback */
	message_callback_t callback;
} message_handler_t;

/*! Initializes the SYSTEM module. This function should be called before any
 * other function in the SYSTEM module */
void SYSTEM_init();
/*! Adds a task to the multitasking loop.
 * \param callback a function of type \ref task_callback_t which should
 * implement the task
 * \param priority task priority. `0` means every cycle, a positive integer
 * `n` means every `n+1` cycles.
 */
void SYSTEM_add_task(task_callback_t callback, int priority);
/*! Runs all added tasks in an infinite loop. This is the last function which
 * should be called from the `main` function. It never returns. */
void SYSTEM_main_loop();

/*! Notify all the subscribers, sending them a message of a particular type, subtype and data
 * \param type one of the message types defined in \ref message_type_t
 * \param subtype an integer in range 0-254
 * \param data any pointer understood and handler properly by the subscribers
 */
int SYSTEM_publish_message(message_type_t type, uint8_t subtype, void *data);
/*! Subscribe to a particular message type. The subscriber will be called only
 * on a message with specified subtype
 * \param type one of the message types defined in \ref message_type_t
 * \param subtype an integer in range 0-254 or \ref ANY signifying
 * subscription to all message subtypes
 * \param a pointer to a function of type \ref message_callback_t to be called
 * when the message is published
 */
int SYSTEM_subscribe(message_type_t type, uint8_t subtype, message_callback_t handler);

/*! @} */
