#ifndef included_btcdeb_instance_h_
#define included_btcdeb_instance_h_

#include <debugger/interpreter.h>
#include <util/strencodings.h>
#include <policy/policy.h>
#include <streams.h>
#include <pubkey.h>
#include <value.h>
#include <vector>
#include <map>

typedef std::vector<unsigned char> valtype;

class Instance {
public:
    InterpreterEnv* env;
    TaprootCommitmentEnv* tce;
    ScriptExecutionData execdata;
    PrecomputedTransactionData txdata;
    int count;
    ECCVerifyHandle evh;
    CTransactionRef tx;
    CTransactionRef txin;
    /*
     * INPUT TX VOUT            TX VIN
     * [0]                      [0]*    <-- txin_index==0
     * [1]*                     [1]
     *    `-- txin_vout_index==1
     */
    int64_t txin_index;             ///< index of the input txid in tx's inputs
    int64_t txin_vout_index;        ///< index inside txin of the output to tx
    std::vector<CAmount> amounts;
    SigVersion sigver;
    CScript script;
    CScript successor_script;
    std::vector<valtype> stack;
    BaseSignatureChecker* checker;
    ScriptError error;
    std::string exception_string = "";
    /**
     * Mapping of pubkeys to signatures that should be considered cryptographically valid.
     *
     * For the keypair <key>=<value>, if a signature <value> is checked against a pubkey <key>
     * for any signature hash <hash>, the cryptographic check is skipped and the signature is
     * assumed to be valid.
     */
    std::map<valtype,valtype> pretend_valid_map;
    std::set<valtype> pretend_valid_pubkeys;
    bool has_preamble;

    Instance()
    : env(nullptr)
    , tce(nullptr)
    , count(0)
    , txin_index(-1)
    , txin_vout_index(-1)
    , sigver(SigVersion::BASE)
    , checker(nullptr)
    , has_preamble(false)
    {}

    ~Instance() {
        delete env;
        delete checker;
    }

    bool parse_transaction(const char* txdata, bool parse_amounts = false);
    bool parse_input_transaction(const char* txdata, int select_index = -1);

    bool parse_script(const char* script_str);
    bool parse_script(const std::vector<uint8_t>& script_data);

    void parse_stack_args(size_t argc, char* const* argv, size_t starting_index);
    void parse_stack_args(const std::vector<const char*> args);

    bool parse_pretend_valid_expr(const char* expr);

    bool configure_tx_txin();

    bool setup_environment(unsigned int flags = STANDARD_SCRIPT_VERIFY_FLAGS);

    bool at_end();
    bool at_start();
    std::string error_string();

    bool step(size_t steps = 1);

    bool rewind();

    bool eval(const size_t argc, char* const* argv);

    uint256 calc_sighash();
};

#endif // included_btcdeb_instance_h_
