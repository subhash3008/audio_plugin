/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

auto getPhaserRateName() { return juce::String("Phaser RateHz"); }
auto getPhaserCenterFreqName() { return juce::String("Phaser Center FreqHz"); }
auto getPhaserDepthName() { return juce::String("Phaser Depth %"); }
auto getPhaserFeedbackName() { return juce::String("Phaser Feedback %"); }
auto getPhaserMixName() { return juce::String("Phaser Mix %"); }

auto getChorusRateName() { return juce::String("Chorus RateHz"); }
auto getChorusDepthName() { return juce::String("Chorus Depth %"); }
auto getChorusCenterDelayName() { return juce::String("Chorus Center Delay Ms"); }
auto getChorusFeedbackName() { return juce::String("Chorus Feedback %"); }
auto getChorusMixName() { return juce::String("Chorus Mix %"); }

auto getOverDriveSaturationName() { return juce::String("Overdrive Saturation"); }

auto getLadderFilterModeName() { return juce::String("Ladder Filter Mode"); }
auto getLadderFilterCutOffFreqName() { return juce::String("Ladder Filter Cut Off FreqHz"); }
auto getLadderFilterResonanceName() { return juce::String("Ladder Filter Resonance"); }
auto getLadderFilterDriveName() { return juce::String("Ladder Filter Drive"); }

auto getLadderFilterChoices()
{
  return juce::StringArray
  {
    "LPF12",  // low pass,  12 dB/octave
    "HPF12",  // high pass, 12 dB/octave
    "BPF12",  // band pass, 12 dB/octave
    "LPF24",  // low pass,  24 dB/octave
    "HPF24",  // high pass, 24 dB/octave
    "BPF24"   // band pass, 24 dB/octave
  };
}

//==============================================================================
Audio_pluginAudioProcessor::Audio_pluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
  auto floatParams = std::array
  {
    &phaserRateHz,
    &phaserCenterFreqHz,
    &phaserDepthPercent,
    &phaserFeedbackPercent,
    &phaserMixPercent,

    &chorusRateHz,
    &chorusDepthPercent,
    &chorusCenterDelayMs,
    &chorusFeedbackPercent,
    &chorusMixPercent,

    &overdriveSaturation,

    &ladderFilterCutOffFreqHz,
    &ladderFilterResonance,
    &ladderFilterDrive
  };

  auto floatNameFuncs = std::array
  {
    &getPhaserRateName,
    &getPhaserCenterFreqName,
    &getPhaserDepthName,
    &getPhaserFeedbackName,
    &getPhaserMixName,

    &getChorusRateName,
    &getChorusDepthName,
    &getChorusCenterDelayName,
    &getChorusFeedbackName,
    &getChorusMixName,

    &getOverDriveSaturationName,

    &getLadderFilterModeName,
    &getLadderFilterCutOffFreqName,
    &getLadderFilterResonanceName,
    &getLadderFilterDriveName
  };

  for (size_t i = 0; i < floatParams.size(); ++i)
  {
    auto ptrToParamPtr = floatParams[i];
    *ptrToParamPtr = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(floatNameFuncs[i]()));

    jassert(*ptrToParamPtr != nullptr);
  }

  ladderFilterMode = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getLadderFilterModeName()));
  jassert(ladderFilterMode != nullptr);
}

Audio_pluginAudioProcessor::~Audio_pluginAudioProcessor()
{
}

//==============================================================================
const juce::String Audio_pluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Audio_pluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Audio_pluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Audio_pluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Audio_pluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Audio_pluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Audio_pluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Audio_pluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Audio_pluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void Audio_pluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Audio_pluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Audio_pluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Audio_pluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

juce::AudioProcessorValueTreeState::ParameterLayout
  Audio_pluginAudioProcessor::createParameterLayout()
{
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  const int versionHint = 1;

  /* Phaser :
  * rate: Hz
  * depth: 0 to 1
  * Center freq: Hz
  * Feedback: -1 to 1
  * Mix: 0 to 1
  */
  /* Phaser Rate in Hz */
  auto name = getPhaserRateName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(0.01f, 2.f, 0.01f, 1.0f),
    0.2f,
    "Hz"
  ));
  /* Phaser Depth: 0 - 1 */
  name = getPhaserDepthName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(0.05f, 1.f, 0.05f, .5f),
    0.2f,
    "Hz"
  ));
  /* Phaser Center Frequency: Audio Hz */
  name = getPhaserCenterFreqName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(20.0f, 20000.f, 1.f, 1.f),
    1000.f,
    "Hz"
  ));
  /* Phaser Feedback Percent: -1 to 1 */
  name = getPhaserFeedbackName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f),
    0.0f,
    "%"
  ));
  /* Phaser Mix: 0 - 1 */
  name = getPhaserMixName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.0f),
    0.05f,
    "%"
  ));

  /* Chorus :
  * Rate: Hz
  * Depth: 0 to 1
  * Center Delay: ms (1-100)
  * Feedback: -1 to 1
  * Mix: 0 to 1
  */
  /* Chorus Rate in Hz */
  name = getChorusRateName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(0.01f, 2.0f, .01f, 1.0f),
    .2f,
    "Hz"
  ));
  /* Chorus Depth: 0 - 1 */
  name = getChorusDepthName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(0.05f, 1.f, 0.05f, .5f),
    0.2f,
    "Hz"
  ));
  /* Chorus Center Delay: Ms 1-100 */
  name = getChorusCenterDelayName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(1.0f, 100.0f, 1.f, 0.f),
    0.0f,
    "Ms"
  ));
  /* Chorus Feedback Percent: -1 to 1 */
  name = getChorusFeedbackName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f),
    0.0f,
    "%"
  ));
  /* Chorus Mix: 0 - 1 */
  name = getChorusMixName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.0f),
    0.05f,
    "%"
  ));

  /* Overdrive:
  * uses the drive filter of the ladder filter class
  * drive: 1-100
  */
  name = getOverDriveSaturationName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f, 1.0f),
    1.0f,
    ""
  ));

  /* Ladder Filter
  * mode: LadderFilterMode enum
  * cut off frequency: Hz
  * resonance: 0-1
  * saturation
  */
  /* Ladder Filter Mode */
  name = getLadderFilterModeName();
  auto choices = getLadderFilterChoices();
  layout.add(std::make_unique<juce::AudioParameterChoice>(
    juce::ParameterID{ name, versionHint },
    name,
    choices,
    0U
  ));
  /* Ladder Filter Cut off frequency */
  name = getLadderFilterCutOffFreqName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 1.0f),
    20000.0f,
    "Hz"
  ));
  /* Ladder Filter Resonance */
  name = getLadderFilterResonanceName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f),
    0.0f,
    ""
  ));
  /* Ladder Filter Drive */
  name = getLadderFilterResonanceName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ name, versionHint },
    name,
    juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f, 1.0f),
    1.0f,
    ""
  ));

  return layout;
}

void Audio_pluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //[DONE]: add APVTS
    
    //TODO: create audio parameters for all dsp choices
    //TODO: update DSP here from audio parameters
    //TODO: save/load the settings
    //TODO: save/load the dsp order
    //TODO: Drag-To_Reorder GUI
    //TODO: GUI design for each dsp instance
    //TODO: metering
    //TODO: prepare all DSP
    //TODO: wet/dry knobs [BONUS]
    //TODO: mono and stereo versions [MONO is BONUS]
    //TODO: modulators [BONUS]
    //TODO: thread-safe filter update [BONUS]
    //TODO: pre/post filtering
    //TODO: delay module [BONUS]

    auto newDspOrder = DSP_Order();

    // try to pull
    while (dspOrderFifo.pull(newDspOrder))
    {

    }

    // if pulled, replace dspOrder
    if (newDspOrder != DSP_Order())
    {
      dspOrder = newDspOrder;
    }

    // convert dspOrder into an array of pointers
    DSP_Pointers dspPointers;
    for (size_t i = 0; i < dspPointers.size(); ++i)
    {
      switch (dspOrder[i])
      {
        case DSP_Option::Phase:
          dspPointers[i] = &phaser;
          break;
        case DSP_Option::Chorus:
          dspPointers[i] = &chorus;
          break;
        case DSP_Option::Overdrive:
          dspPointers[i] = &overdrive;
          break;
        case DSP_Option::LadderFilter:
          dspPointers[i] = &ladderFilter;
          break;
        case DSP_Option::END_OF_LIST:
          jassertfalse;
          break;
      }
    }

    // process
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    for (size_t i = 0; i < dspPointers.size(); ++i)
    {
      if (nullptr != dspPointers[i])
      {
        dspPointers[i]->process(context);
      }
    }
}

//==============================================================================
bool Audio_pluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Audio_pluginAudioProcessor::createEditor()
{
    return new Audio_pluginAudioProcessorEditor (*this);
}

//==============================================================================
void Audio_pluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Audio_pluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Audio_pluginAudioProcessor();
}
