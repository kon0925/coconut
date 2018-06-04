/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2011, 2016 Oracle and/or its affiliates.  All rights reserved.
 *
 * $Id$
 */

package com.sleepycat.db;

import com.sleepycat.db.internal.DbChannel;

/**
A ReplicationChannel handle is used to manage a channel in a replication group.
ReplicationChannel handles are opened using the {@link com.sleepycat.db.Environment#openChannel Environment.openChannel} method.
*/
public class ReplicationChannel {
    private DbChannel chan;

    /* package */
    ReplicationChannel(final DbChannel chan) {
	this.chan = chan;
    }

    /**
    Close the channel.
    @throws DatabaseException if a failure occurs.
    */
    public void close()
        throws DatabaseException {

        this.chan.close(0);
    }

    /**
    Send a message on the message channel asynchronously.
    <p>
    @param messages the messages
    @throws DatabaseException if a failure occurs.
    */
    public void sendMessage(java.util.Set messages) 
        throws DatabaseException {

        DatabaseEntry[] msgs = (DatabaseEntry[])messages.toArray();
        this.chan.send_repmsg(msgs, msgs.length, 0);
    }

    /**
    Send request on the message channel. It blocks waiting for a response 
    before returning.
    <p>
    @param messages the messages
    @param response the response
    @param timeout the timeout
    @throws DatabaseException if a failure occurs.
    */
    public void sendRequest(
        java.util.Set messages, DatabaseEntry response, long timeout) 
        throws DatabaseException {

        DatabaseEntry[] msgs = (DatabaseEntry[])messages.toArray();
        this.chan.send_request(msgs, msgs.length, response, timeout, 0);
    }

    /**
    Sets the default timeout value for the channel.
    <p>
    @param timeout the timeout
    @throws DatabaseException if a failure occurs.
    */
    public void setTimeout(long timeout) 
        throws DatabaseException {

        this.chan.set_timeout(timeout);
    }
}
