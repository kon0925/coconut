/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2002, 2016 Oracle and/or its affiliates.  All rights reserved.
 *
 * $Id$
 */

package com.sleepycat.client;

import com.sleepycat.thrift.TDbt;

/**
 * Encodes database key and data items as a byte array.
 * <p>
 * Storage and retrieval for the {@link SDatabase} and {@link SCursor} methods
 * are based on key/data pairs. Both key and data items are represented by
 * SDatabaseEntry objects. Key and data byte arrays may refer to arrays of zero
 * length up to arrays of essentially unlimited length. A key item may also
 * refer to a record number, which is a 32-bit integer. In this case, the
 * record number must be accessed through {@link #setRecordNumber} and {@link
 * #getRecordNumber} instead of setData and getData.
 * <p>
 * The SDatabaseEntry class provides simple access to an underlying object
 * whose elements can be examined or changed. SDatabaseEntry objects can be
 * subclassed, providing a way to associate with it additional data or
 * references to other structures.
 * <p>
 * Access to SDatabaseEntry objects is not re-entrant. In particular, if
 * multiple threads simultaneously access the same SDatabaseEntry object using
 * {@link SDatabase} or {@link SCursor} methods, the results are undefined.
 * <p>
 * <h3>Input and Output Parameters</h3>
 * <p>
 * SDatabaseEntry objects are used for both input data (when writing to a
 * database or specifying a search parameter) and output data (when reading
 * from a database). For certain methods, one parameter may be an input
 * parameter and another may be an output parameter. For example, the {@link
 * SDatabase#get} method has an input key parameter and an output data
 * parameter. The documentation for each method describes whether its
 * parameters are input or output parameters.
 * <p>
 * For SDatabaseEntry input parameters, the caller is responsible for
 * initializing the data array of the SDatabaseEntry. For SDatabaseEntry output
 * parameters, the method called will initialize the data array.
 * <p>
 * <h3>Partial Offset and Length Properties</h3>
 * <p>
 * By default the specified data (byte array, offset and size) corresponds to
 * the full stored key or data item. Optionally, the Partial property can be
 * set to true, and the PartialOffset and PartialLength properties are used to
 * specify the portion of the key or data item to be read or written. For
 * details, see the {@link #setPartial} method.
 * <p>
 * Note that the Partial properties are set only by the caller. They will never
 * be set by a {@link SDatabase} or {@link SCursor} method. Therefore, the
 * application can assume that the Partial properties are not set, unless the
 * application itself sets them explicitly.
 */
public class SDatabaseEntry extends ThriftWrapper<TDbt, TDbt._Fields>
        implements SDatabaseEntryBase {
    /**
     * Construct a SDatabaseEntry with null data.
     */
    public SDatabaseEntry() {
        this((byte[]) null);
    }

    /**
     * Construct a SDatabaseEntry with a given byte array.
     *
     * @param data byte array wrapped by the SDatabaseEntry
     */
    public SDatabaseEntry(byte[] data) {
        super(new TDbt().setData(data));
        setPartial(0, 0, false);
    }

    SDatabaseEntry(TDbt dbt) {
        super(dbt);
    }

    void setDataFromTDbt(TDbt dbt) {
        if (dbt != null) {
            if (dbt.isSetData()) {
                setData(dbt.getData());
            } else if (dbt.isSetRecordNumber()) {
                setRecordNumber(dbt.getRecordNumber());
            }
        }
    }

    /**
     * Return whether this SDatabaseEntry is configured to be stored as a blob.
     *
     * @return whether this SDatabaseEntry is configured to be stored as a blob
     */
    public boolean getBlob() {
        return (boolean) getField(TDbt._Fields.BLOB);
    }

    /**
     * Configure this SDatabaseEntry to be stored as a blob.
     *
     * @param blob whether this SDatabaseEntry is configured to be stored as a
     * blob
     * @return this
     */
    public SDatabaseEntry setBlob(final boolean blob) {
        getThriftObj().setBlob(blob);
        return this;
    }

    /**
     * Return the byte array.
     * <p>
     * For a SDatabaseEntry that is used as an output parameter, the byte
     * array will always be a newly allocated array. The byte array specified
     * by the caller will not be used and may be null.
     * <p>
     * It is an error to get the record number using this method. Record
     * numbers must be get using {@link #getRecordNumber}.
     *
     * @return the byte array
     */
    public byte[] getData() {
        return (byte[]) getField(TDbt._Fields.DATA);
    }

    /**
     * Sets the byte array.
     * <p>
     * It is an error to set a record number using this method. Record numbers
     * must be set using {@link #setRecordNumber}.
     *
     * @param data byte array wrapped by the SDatabaseEntry
     * @return this
     */
    public SDatabaseEntry setData(final byte[] data) {
        getThriftObj().unsetRecordNumber();
        getThriftObj().setData(data);
        return this;
    }

    /**
     * Return the byte length of the partial record being read or written by
     * the application, in bytes.
     * <p>
     * Note that the Partial properties are set only by the caller.  They
     * will never be set by a {@link SDatabase} or {@link SCursor} method.
     *
     * @return the byte length of the partial record being read or written by
     * the application, in bytes
     * @see #setPartial
     */
    public int getPartialLength() {
        return (int) getField(TDbt._Fields.PARTIAL_LENGTH);
    }

    /**
     * Return the offset of the partial record being read or written by the
     * application, in bytes.
     * <p>
     * Note that the Partial properties are set only by the caller.  They
     * will never be set by a {@link SDatabase} or {@link SCursor} method.
     *
     * @return the offset of the partial record being read or written by the
     * application, in bytes
     * @see #setPartial
     */
    public int getPartialOffset() {
        return (int) getField(TDbt._Fields.PARTIAL_OFFSET);
    }

    /**
     * Return whether this SDatabaseEntry is configured to read or write
     * partial records.
     * <p>
     * Note that the Partial properties are set only by the caller.  They
     * will never be set by a {@link SDatabase} or {@link SCursor} method.
     *
     * @return whether this SDatabaseEntry is configured to read or write
     * partial records.
     * @see #setPartial
     */
    public boolean getPartial() {
        return (boolean) getField(TDbt._Fields.PARTIAL);
    }

    /**
     * Configure this SDatabaseEntry to read or write partial records.
     * <p>
     * Do partial retrieval or storage of an item.  If the calling application
     * is doing a retrieval, length bytes specified by {@code dlen}, starting
     * at the offset set by {@code doff} bytes from the beginning of the
     * retrieved data record are returned as if they comprised the entire
     * record.  If any or all of the specified bytes do not exist in the
     * record, the get is successful, and any existing bytes are returned.
     * <p>
     * For example, if the data portion of a retrieved record was 100 bytes,
     * and a partial retrieval was done using a SDatabaseEntry having a partial
     * length of 20 and a partial offset of 85, the retrieval would succeed and
     * the retrieved data would be the last 15 bytes of the record.
     * <p>
     * If the calling application is storing an item, length bytes specified by
     * {@code dlen}, starting at the offset set by {@code doff} bytes from the
     * beginning of the specified key's data item are replaced by the data
     * specified by the SDatabaseEntry.  If the partial length is smaller than
     * the data, the record will grow; if the partial length is larger than the
     * data, the record will shrink.  If the specified bytes do not exist, the
     * record will be extended using nul bytes as necessary, and the store will
     * succeed.
     * <p>
     * It is an error to specify a partial key when performing a put operation
     * of any kind.
     * <p>
     * It is an error to attempt a partial store using the {@link
     * SDatabase#put} method in a database that supports duplicate records.
     * Partial stores in databases supporting duplicate records must be done
     * using a cursor method.
     * <p>
     * Note that the Partial properties are set only by the caller.  They
     * will never be set by a {@link SDatabase} or {@link SCursor} method.
     *
     * @param doff the offset of the partial record being read or written by
     * the application, in bytes
     * @param dlen the byte length of the partial record being read or written
     * by the application, in bytes
     * @param partial whether this SDatabaseEntry is configured to read or
     * write partial records
     * @return this
     */
    public SDatabaseEntry setPartial(final int doff, final int dlen,
            final boolean partial) {
        getThriftObj().setPartial(partial);
        if (partial) {
            getThriftObj().setPartialLength(dlen);
            getThriftObj().setPartialOffset(doff);
        } else {
            getThriftObj().unsetPartialLength();
            getThriftObj().unsetPartialOffset();
        }
        return this;
    }

    /**
     * Return the record number in this entry.
     *
     * @return the decoded record number
     */
    public int getRecordNumber() {
        return (int) getField(TDbt._Fields.RECORD_NUMBER);
    }

    /**
     * Initialize the entry from a logical record number.  Record numbers
     * are integer keys starting at 1.
     *
     * @param recno the record number to be encoded
     * @return this
     */
    public SDatabaseEntry setRecordNumber(final int recno) {
        getThriftObj().unsetData();
        getThriftObj().setRecordNumber(recno);
        return this;
    }
}
