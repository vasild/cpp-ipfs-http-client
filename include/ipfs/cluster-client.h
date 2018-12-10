
#ifndef IPFS_CLUSTER_CLIENT_H
#define IPFS_CLUSTER_CLIENT_H

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
class Cluster {
 public:
  /** Constructor.
   *
   * An example usage:
   * @snippet generic.cc ipfs::Cluster::Cluster
   *
   * @since version 0.1.0 */
  Cluster(
      /** [in] Hostname or IP address of the server to connect to. */
      const std::string& host,
      /** [in] Port to connect to. */
      long port);

  /** Destructor.
   * @since version 0.1.0 */
  ~Cluster();

  /** Return the identity of the peer.
   *
   * Implements
   * https://github.com/ipfs/interface-ipfs-core/tree/master/API/generic#id.
   *
   * An example usage:
   * @snippet generic.cc ipfs::Cluster::Id
   *
   * @throw std::exception if any error occurs
   *
   * @since version 0.1.0 */
  void Id(
      /** [out] The identity of the peer. It contains at least the properties
       * "Addresses", "ID", "PublicKey". */
      Json* id);

  void Version(
      /** [out] The peer's implementation version. It contains at least the
       * properties "Repo", "System", "Version". */
      Json* version);

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
      const std::vector<ipfs::http::FileUpload>& files,
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

#endif /* IPFS_CLUSTER_CLIENT_H */
