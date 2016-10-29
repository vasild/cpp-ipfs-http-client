/* Copyright (c) 2016-2016, Vasil Dimov

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef IPFS_CLIENT_H
#define IPFS_CLIENT_H

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include <ipfs/http/transport.h>

/** IPFS namespace. Everything IPFS related goes inside it. */
namespace ipfs {

/** Type of the output of some methods, aliased for convenience.
 * @see https://github.com/nlohmann/json */
using Json = nlohmann::json;

/** IPFS client.
 *
 * It implements the interface described in
 * https://github.com/ipfs/interface-ipfs-core.
 * The methods of this class may throw some variant of `std::exception` if a
 * connectivity error occurs or if the response cannot be parsed. Be prepared!
 *
 * @since version 1.0.0 */
class Client {
 public:
  /** Constructor.
   *
   * An example usage:
   * @snippet simple.cc ipfs::Client::Client
   *
   * @since version 1.0.0 */
  Client(
      /** [in] Hostname or IP address of the server to connect to. */
      const std::string& host,
      /** [in] Port to connect to. */
      long port);

  /** Destructor.
   * @since version 1.0.0 */
  ~Client();

  /** Return the identity of the peer.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/generic#id.
   *
   * An example usage:
   * @snippet simple.cc ipfs::Client::Id
   *
   * @throw std::exception if any error occurs
   *
   * @since version 1.0.0 */
  void Id(
      /** [out] The identity of the peer. It contains at least the properties
       * "Addresses", "ID", "PublicKey". */
      Json* id);

  /** Return the implementation version of the peer.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/generic#version.
   *
   * An example usage:
   * @snippet simple.cc ipfs::Client::Version
   *
   * @throw std::exception if any error occurs
   *
   * @since version 1.0.0 */
  void Version(
      /** [out] The peer's implementation version. It contains at least the
       * properties "Repo", "System", "Version". */
      Json* version);

  /** Query the current config of the peer.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/config#configget.
   *
   * An example usage:
   * @snippet simple.cc ipfs::Client::ConfigGet
   *
   * @throw std::exception if any error occurs
   *
   * @since version 1.0.0 */
  void ConfigGet(
      /** [in] The key of the value to fetch from the config. If this is an
       * empty string, then the whole config is fetched. */
      const std::string& key,
      /** [out] Fetched config. */
      Json* config);

  /** Get a raw IPFS block.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/block#get.
   *
   * An example usage:
   * @snippet simple.cc ipfs::Client::BlockGet
   *
   * @throw std::exception if any error occurs
   *
   * @since version 1.0.0 */
  void BlockGet(
      /** [in] Id of the block (multihash). */
      const std::string& block_id,
      /** [out] Raw contents of the block is written to this stream as it is
       * retrieved. */
      std::iostream* block);

  /** Store a raw block in IPFS.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/block#put.
   *
   * An example usage:
   * @snippet simple.cc ipfs::Client::BlockPut
   *
   * @throw std::exception if any error occurs
   *
   * @since version 1.0.0 */
  void BlockPut(
      /** [in] Raw contents of the block to store. */
      const http::FileUpload& block,
      /** [out] Information about the stored block. */
      Json* stat);

  /** Get information for a raw IPFS block.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/block#stat.
   *
   * An example usage:
   * @snippet simple.cc ipfs::Client::BlockStat
   *
   * @throw std::exception if any error occurs
   *
   * @since version 1.0.0 */
  void BlockStat(
      /** [in] Id of the block (multihash). */
      const std::string& block_id,
      /** [out] Retrieved information about the block. */
      Json* stat);

  /** Get a file from IPFS.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/files#get.
   *
   * An example usage:
   * @snippet simple.cc ipfs::Client::FilesGet
   *
   * @throw std::exception if any error occurs
   *
   * @since version 1.0.0 */
  void FilesGet(
      /** [in] Path of the file in IPFS. For example:
       * `"/ipfs/QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG/readme"` */
      const std::string& path,
      /** [out] The file's contents is written to this stream as it is retrieved
       * from IPFS. */
      std::iostream* response);

  /** Add files to IPFS.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/files#add.
   *
   * An example usage:
   * @snippet simple.cc ipfs::Client::FilesAdd
   *
   * @throw std::exception if any error occurs
   *
   * @since version 1.0.0 */
  void FilesAdd(
      /** [in] List of files to add. */
      const std::vector<http::FileUpload>& files,
      /** [out] List of results, one per file. For example:
       * [{"path": "foo.txt", "hash": "Qm...", "size": 123}, {"path": ...}, ...]
       */
      Json* result);

 private:
  /** Fetch any URL that returns JSON and parse it into `response`. */
  void FetchAndParseJson(
      /** [in] URL to fetch. For example:
       * `"http://localhost:5001/api/v0/version"` but can be anything. */
      const std::string& url,
      /** [out] Parsed JSON response. */
      Json* response);

  /** Parse a string into a JSON. It just calls Json::parse() and appends the
   * input to the error message in case of an error.
   *
   * @throw std::exception if any error occurs */
  static void ParseJson(
      /** [in] String to parse. */
      const std::string& input,
      /** [out] Parse result. */
      Json* result);

  /** The URL prefix of our peer. Crafted from `host` and `port` constructor's
   * arguments. For example: `"http://localhost:5001/api/v0"`. */
  std::string url_prefix_;

  /** The underlying transport. */
  http::Transport* http_;
};
} /* namespace ipfs */

#endif /* IPFS_CLIENT_H */
