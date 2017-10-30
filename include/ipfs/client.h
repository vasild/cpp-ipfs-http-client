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
#include <utility>
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
 * @since version 0.1.0 */
class Client {
 public:
  /** Constructor.
   *
   * An example usage:
   * @snippet generic.cc ipfs::Client::Client
   *
   * @since version 0.1.0 */
  Client(
      /** [in] Hostname or IP address of the server to connect to. */
      const std::string& host,
      /** [in] Port to connect to. */
      long port);

  /** Destructor.
   * @since version 0.1.0 */
  ~Client();

  /** Return the identity of the peer.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/generic#id.
   *
   * An example usage:
   * @snippet generic.cc ipfs::Client::Id
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
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
   * @snippet generic.cc ipfs::Client::Version
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
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
   * @snippet config.cc ipfs::Client::ConfigGet
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ConfigGet(
      /** [in] The key of the value to fetch from the config. If this is an
       * empty string, then the whole config is fetched. */
      const std::string& key,
      /** [out] Fetched config. */
      Json* config);

  /** Add or replace a config knob at the peer.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/config#configset.
   *
   * An example usage:
   * @snippet config.cc ipfs::Client::ConfigSet
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ConfigSet(
      /** [in] The key of the config knob to set. */
      const std::string& key,
      /** [in] The value to set for the key. */
      const Json& value);

  /** Replace the entire config at the peer.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/config#configreplace.
   *
   * An example usage:
   * @snippet config.cc ipfs::Client::ConfigReplace
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ConfigReplace(
      /** [in] The entire config to set/replace. */
      const Json& config);

  /** Retrieve the peer info of a reachable node in the network.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/dht#findpeer.
   *
   * An example usage:
   * @snippet dht.cc ipfs::Client::DhtFindPeer
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void DhtFindPeer(
      /** [in] Id of the peer (multihash). */
      const std::string& peer_id,
      /** [out] List of the peer's addresses. */
      Json* addresses);

  /** Retrieve the providers for a content that is addressed by a hash.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/dht#findprovs.
   *
   * An example usage:
   * @snippet dht.cc ipfs::Client::DhtFindProvs
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void DhtFindProvs(
      /** [in] Multihash whose providers to find. */
      const std::string& hash,
      /** [out] List of providers of `hash`. */
      Json* providers);

  /** Get a raw IPFS block.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/block#get.
   *
   * An example usage:
   * @snippet block.cc ipfs::Client::BlockGet
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
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
   * @snippet block.cc ipfs::Client::BlockPut
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
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
   * @snippet block.cc ipfs::Client::BlockStat
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
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
   * @snippet files.cc ipfs::Client::FilesGet
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
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
   * @snippet files.cc ipfs::Client::FilesAdd
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void FilesAdd(
      /** [in] List of files to add. */
      const std::vector<http::FileUpload>& files,
      /** [out] List of results, one per file. For example:
       * [{"path": "foo.txt", "hash": "Qm...", "size": 123}, {"path": ...}, ...]
       */
      Json* result);

  /** List directory contents for Unix filesystem objects.
   *
   * Implements
   * https://ipfs.io/docs/api/#api-v0-file-ls
   *
   * An example usage:
   * @snippet files.cc ipfs::Client::FilesLs
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.2.0 */
  void FilesLs(
      /** [in] The path to an IPFS object. */
      const std::string& path,
      /** [out] List of results (files). For example:
      { "Arguments": { ... },
          "Objects": {
              "QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG": {
                  "Hash": "QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG",
                  "Links": [
                      {
                          "Hash": "QmZTR5bcpQD7cFgTorqxZDYaew1Wqgf...",
                          "Name": "about",
                          "Size": 1677,
                          "Type": "File"
                      },
                      ...
                      {
                          "Hash": "QmdncfsVm2h5Kqq9hPmU7oAVX2zTSVP...",
                          "Name": "quick-start",
                          "Size": 1717,
                          "Type": "File"
                      },
                      ...
                  ],
                  "Size": 0,
                  "Type": "Directory"
              }
          }
      }
      */
      Json* result);

  /** Create a new MerkleDAG node.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/object#objectnew.
   *
   * An example usage:
   * @snippet object.cc ipfs::Client::ObjectNew
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ObjectNew(
      /** [out] Id of the newly created object (multihash). */
      std::string* object_id);

  /** Store a MerkleDAG node.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/object#objectput.
   *
   * An example usage:
   * @snippet object.cc ipfs::Client::ObjectPut
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ObjectPut(
      /** [in] Node to store. */
      const Json& object,
      /** [out] Stored node. Should be the same as the provided `object` plus
       * the stored object's multihash id. */
      Json* object_stored);

  /** Get a MerkleDAG node.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/object#objectget.
   *
   * An example usage:
   * @snippet object.cc ipfs::Client::ObjectGet
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ObjectGet(
      /** [in] Id (multihash) of the MerkleDAG node to fetch. */
      const std::string& object_id,
      /** [out] Retrieved MerkleDAG node. For example:
       * {"Data": "abc", "Links": [{"Name": "link1", "Hash": "...", "Size": 8}]}
       */
      Json* object);

  /** Get the data field of a MerkleDAG node.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/object#objectdata.
   *
   * An example usage:
   * @snippet object.cc ipfs::Client::ObjectData
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ObjectData(
      /** [in] Id (multihash) of the MerkleDAG node whose data to fetch. */
      const std::string& object_id,
      /** [out] Raw data of the MerkleDAG node. */
      std::string* data);

  /** Get links of a MerkleDAG node.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/object#objectlinks.
   *
   * An example usage:
   * @snippet object.cc ipfs::Client::ObjectLinks
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ObjectLinks(
      /** [in] Id of the object to query (multihash). */
      const std::string& object_id,
      /** [out] Links of the object. For example:
       * [{"Name": "...", "Hash": "...", "Size": 8}, ...] */
      Json* links);

  /** Get stats about a MerkleDAG node.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/object#objectstat.
   *
   * An example usage:
   * @snippet object.cc ipfs::Client::ObjectStat
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ObjectStat(
      /** [in] Id of the object to query (multihash). */
      const std::string& object_id,
      /** [out] Stats about the object. For example:
       * {"NumLinks": 0, "BlockSize": 10, "LinksSize": 2, ...} */
      Json* stat);

  /** Create a new object from an existing MerkleDAG node and add to its links.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/object#objectpatchaddlink.
   *
   * An example usage:
   * @snippet object.cc ipfs::Client::ObjectPatchAddLink
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ObjectPatchAddLink(
      /** [in] Id of the object to clone (multihash). */
      const std::string& source,
      /** [in] Link name. */
      const std::string& link_name,
      /** [in] Id of the object that the link points to (multihash). */
      const std::string& link_target,
      /** [out] Id of the newly created (cloned) object (multihash). */
      std::string* cloned);

  /** Create a new object from an existing MerkleDAG node and remove one of its
   * links.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/object#objectpatchrmlink.
   *
   * An example usage:
   * @snippet object.cc ipfs::Client::ObjectPatchRmLink
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ObjectPatchRmLink(
      /** [in] Id of the object to remove the link from (multihash). */
      const std::string& source,
      /** [in] Name of the link to remove. */
      const std::string& link_name,
      /** [out] Id of the newly created (cloned) object (multihash). */
      std::string* cloned);

  /** Create a new object from an existing MerkleDAG node and append data to it.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/object#objectpatchappenddata.
   *
   * An example usage:
   * @snippet object.cc ipfs::Client::ObjectPatchAppendData
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ObjectPatchAppendData(
      /** [in] Id of the object to append data to (multihash). */
      const std::string& source,
      /** [in] Data to be appended. */
      const http::FileUpload& data,
      /** [out] Id of the newly created (cloned) object (multihash). */
      std::string* cloned);

  /** Create a new object from an existing MerkleDAG node and set its data.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/object#objectpatchsetdata.
   *
   * An example usage:
   * @snippet object.cc ipfs::Client::ObjectPatchSetData
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void ObjectPatchSetData(
      /** [in] Id of the object whose data to set (multihash). */
      const std::string& source,
      /** [in] Data to be set. */
      const http::FileUpload& data,
      /** [out] Id of the newly created (cloned) object (multihash). */
      std::string* cloned);

  /** Pin a given IPFS object.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/pin#add.
   *
   * An example usage:
   * @snippet pin.cc ipfs::Client::PinAdd
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void PinAdd(
      /** [in] Id of the object to pin (multihash). */
      const std::string& object_id);

  /** List all the objects pinned to local storage.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/pin#ls.
   *
   * An example usage:
   * @snippet pin.cc ipfs::Client::PinLs__a
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void PinLs(
      /** [out] List of pinned objects. */
      Json* pinned);

  /** List the objects pinned under a specific hash.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/pin#ls.
   *
   * An example usage:
   * @snippet pin.cc ipfs::Client::PinLs__b
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void PinLs(
      /** [in] Id of the object to list (multihash). */
      const std::string& object_id,
      /** [out] List of pinned objects. */
      Json* pinned);

  /** Options to control the `PinRm()` method. */
  enum class PinRmOptions {
    /** Just unpin the specified object. */
    NON_RECURSIVE,
    /** Recursively unpin the objects. */
    RECURSIVE,
  };

  /** Unpin an object.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/pin#rm.
   *
   * An example usage:
   * @snippet pin.cc ipfs::Client::PinRm
   *
   * @throw std::exception if any error occurs
   *
   * @sa `PinRmOptions`
   *
   * @since version 0.1.0 */
  void PinRm(
      /** [in] Id of the object to unpin (multihash). */
      const std::string& object_id,
      /** [in] Unpin options. */
      PinRmOptions options);

  /** List of known addresses of each peer connected.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/swarm#addrs.
   *
   * An example usage:
   * @snippet swarm.cc ipfs::Client::SwarmAddrs
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void SwarmAddrs(
      /** [out] The retrieved list. */
      Json* addresses);

  /** Open a connection to a given address.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/swarm#connect.
   *
   * An example usage:
   * @snippet swarm.cc ipfs::Client::SwarmConnect
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void SwarmConnect(
      /** [in] Peer to connect to. For example:
       * `"/ip4/104.131.131.82/tcp/4001/ipfs/QmaCpDMGvV2BGHeYERUEnRQAwe3N8SzbUtfsmvsqQLuvuJ"`
       */
      const std::string& peer);

  /** Close a connection on a given address.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/swarm#disconnect.
   *
   * An example usage:
   * @snippet swarm.cc ipfs::Client::SwarmDisconnect
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void SwarmDisconnect(
      /** [in] Peer to disconnect from to. For example:
       * `"/ip4/104.131.131.82/tcp/4001/ipfs/QmaCpDMGvV2BGHeYERUEnRQAwe3N8SzbUtfsmvsqQLuvuJ"`
       */
      const std::string& peer);

  /** List the peers that we have connections with.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/swarm#peers.
   *
   * An example usage:
   * @snippet swarm.cc ipfs::Client::SwarmPeers
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void SwarmPeers(
      /** [out] The retrieved list. */
      Json* peers);

 private:
  /** Fetch any URL that returns JSON and parse it into `response`. */
  void FetchAndParseJson(
      /** [in] URL to fetch. For example:
       * `"http://localhost:5001/api/v0/version"` but can be anything. */
      const std::string& url,
      /** [out] Parsed JSON response. */
      Json* response);

  /** Submit some files to an URL that returns JSON and parse it into
   * `response`. */
  void FetchAndParseJson(
      /** [in] URL to submit the files to. */
      const std::string& url,
      /** [in] List of files to submit. */
      const std::vector<http::FileUpload>& files,
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

  /** Get a JSON property. Throw an exception with an elaborate explanation
   * if the property does not exist in the JSON.
   *
   * @throw std::exception if any error occurs */
  template <class PropertyType>
  static void GetProperty(
      /** [in] JSON whose property to fetch. */
      const Json& input,
      /** [in] Property name. */
      const std::string& property_name,
      /** [in] Line number to add to the error message. */
      size_t line_number,
      /** [out] Property value. */
      PropertyType* property_value);

  /** Construct a full URL. The URL is constructed from url_prefix_, path and
   * the provided parameters (if any). For example:
   * http://l:5001/api/v0 / block/get ?stream-channels=true& foo = bar &...
   * ^ `url_prefix_`        ^ `path`                         ^ [1] ^ [2]
   * [1] parameters[0].first
   * [2] parameters[0].second.
   * @return The full URL. */
  std::string MakeUrl(
      /** Path to use after `url_prefix_`. For example "block/get". */
      const std::string& path,
      /** List of parameters. In the form of (name, value) for each. */
      const std::vector<std::pair<std::string, std::string>>& parameters = {});

  /** The URL prefix of our peer. Crafted from `host` and `port` constructor's
   * arguments. For example: `"http://localhost:5001/api/v0"`. */
  std::string url_prefix_;

  /** The underlying transport. */
  http::Transport* http_;
};
} /* namespace ipfs */

#endif /* IPFS_CLIENT_H */
