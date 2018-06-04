/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2002, 2016 Oracle and/or its affiliates.  All rights reserved.
 *
 * $Id$
 */

package com.sleepycat.client;

import org.junit.Test;

import java.io.IOException;
import java.nio.file.Files;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

public class BdbServerConnectionTest extends ClientTestBase {

    @Test
    public void testOpenEnvironment() throws Exception {
        SEnvironment env = connection.openEnvironment("env",
                new SEnvironmentConfig().setAllowCreate(true));
        env.close();

        assertThat(Files.isDirectory(testRoot.resolve("env")), is(true));
    }

    @Test(expected = IOException.class)
    public void testOpenEnvironmentNonExist() throws Exception {
        connection.openEnvironment("env", null);
    }
}