/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2000, 2016 Oracle and/or its affiliates.  All rights reserved.
 *
 */

package com.sleepycat.collections;

import com.sleepycat.util.ExceptionUnwrapper;

/**
 * The interface implemented to perform the work within a transaction.
 * To run a transaction, an instance of this interface is passed to the
 * {@link TransactionRunner#run} method.
 *
 * @author Mark Hayes
 */
public interface TransactionWorker {

    /**
     * Perform the work for a single transaction.
     *
     * @throws Exception the exception to be thrown to the caller of
     * {@link TransactionRunner#run(TransactionWorker)}. The exception will
     * first be unwrapped by calling {@link ExceptionUnwrapper#unwrap}, and the
     * transaction will be aborted.
     *
     * @see TransactionRunner#run
     */
    void doWork()
        throws Exception;
}
