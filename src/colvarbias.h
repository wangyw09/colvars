// -*- c++ -*-

#ifndef COLVARBIAS_H
#define COLVARBIAS_H

#include "colvar.h"
#include "colvarparse.h"
#include "colvardeps.h"


/// \brief Collective variable bias, base class
class colvarbias
  : public virtual colvarparse, public virtual colvardeps {
public:

  /// Name of this bias
  std::string name;

  /// Type of this bias
  std::string bias_type;

  /// If there is more than one bias of this type, record its rank
  int rank;

  /// Add a new collective variable to this bias
  int add_colvar(std::string const &cv_name);

  /// How many variables are defined for this bias
  inline size_t num_variables() const
  {
    return colvars.size();
  }

  /// Access the variables vector
  inline std::vector<colvar *> *variables()
  {
    return &colvars;
  }

  /// Access the i-th variable
  inline colvar * variables(int i) const
  {
    return colvars[i];
  }

  /// Retrieve colvar values and calculate their biasing forces
  /// Return bias energy
  virtual int update();

  /// \brief Compute the energy of the bias with alternative values of the
  /// collective variables (suitable for bias exchange)
  virtual int calc_energy(std::vector<colvarvalue> const &values =
                          std::vector<colvarvalue>(0))
  {
    cvm::error("Error: calc_energy() not implemented.\n", COLVARS_NOT_IMPLEMENTED);
    return COLVARS_NOT_IMPLEMENTED;
  }

  /// Send forces to the collective variables
  virtual void communicate_forces();

  /// Load new configuration - force constant and/or centers only
  virtual int change_configuration(std::string const &conf);

  /// Calculate change in energy from using alternate configuration
  virtual cvm::real energy_difference(std::string const &conf);

  /// Give the total number of bins for a given bias.
  // FIXME this is currently 1D only
  virtual int bin_num();
  /// Calculate the bin index for a given bias.
  // FIXME this is currently 1D only
  virtual int current_bin();
  //// Give the count at a given bin index.
  // FIXME this is currently 1D only
  virtual int bin_count(int bin_index);
  //// Share information between replicas, whatever it may be.
  virtual int replica_share();

  /// Perform analysis tasks
  virtual void analyze() {}

  /// \brief Constructor
  colvarbias(char const *key);

  /// \brief Parse config string and (re)initialize
  virtual int init(std::string const &conf);

  /// \brief Set to zero all mutable data
  virtual int reset();

private:

  /// Default constructor
  colvarbias();

  /// Copy constructor
  colvarbias(colvarbias &);

public:

  /// \brief Delete everything
  virtual int clear();

  /// Destructor
  virtual ~colvarbias();

  /// Write the values of specific mutable properties to a string
  virtual std::string const get_state_params() const;

  /// Read the values of specific mutable properties from a string
  virtual int set_state_params(std::string const &state_conf);

  /// Write all mutable data not already written by get_state_params()
  virtual std::ostream & write_state_data(std::ostream &os)
  {
    return os;
  }

  /// Read all mutable data not already set by set_state_params()
  virtual std::istream & read_state_data(std::istream &is)
  {
    return is;
  }

  /// Read a keyword from the state data (typically a header)
  std::istream & read_state_data_key(std::istream &is, char const *key);

  /// Write the bias configuration to a restart file or other stream
  virtual std::ostream & write_state(std::ostream &os);

  /// Read the bias configuration from a restart file or other stream
  virtual std::istream & read_state(std::istream &is);

  /// Write a label to the trajectory file (comment line)
  virtual std::ostream & write_traj_label(std::ostream &os);

  /// Output quantities such as the bias energy to the trajectory file
  virtual std::ostream & write_traj(std::ostream &os);

  /// (Re)initialize the output files (does not write them yet)
  virtual int setup_output()
  {
    return COLVARS_OK;
  }

  /// Write any output files that this bias may have (e.g. PMF files)
  virtual int write_output_files()
  {
    return COLVARS_OK;
  }

  /// Use this prefix for all output files
  std::string output_prefix;

  /// If this bias is communicating with other replicas through files, send it to them
  virtual int write_state_to_replicas()
  {
    return COLVARS_OK;
  }

  inline cvm::real get_energy()
  {
    return bias_energy;
  }

  /// \brief Implementation of the feature list for colvarbias
  static std::vector<feature *> cvb_features;

  /// \brief Implementation of the feature list accessor for colvarbias
  virtual const std::vector<feature *> &features()
  {
    return cvb_features;
  }
  virtual std::vector<feature *> &modify_features()
  {
    return cvb_features;
  }

protected:

  /// \brief Pointers to collective variables to which the bias is
  /// applied; current values and metric functions will be obtained
  /// through each colvar object
  std::vector<colvar *>    colvars;

  /// \brief Current forces from this bias to the variables
  std::vector<colvarvalue> colvar_forces;

  /// \brief Current energy of this bias (colvar_forces should be obtained by deriving this)
  cvm::real                bias_energy;

  /// Whether to write the current bias energy from this bias to the trajectory file
  bool                     b_output_energy;

  /// \brief Whether this bias has already accumulated information
  /// (for history-dependent biases)
  bool                     has_data;

  /// \brief Step number read from the last state file
  size_t                   state_file_step;

};

#endif
