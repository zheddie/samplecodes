// Example 1: sets up service wrapper, sends initial message, and
// receives response.
var prompt = require('prompt-sync')();
var ConversationV1 = require('watson-developer-cloud/conversation/v1');

// Set up Conversation service wrapper.
var conversation = new ConversationV1({
  username: 'd270e7e5-45e5-4032-9b6d-db325423c8f5', // replace with username from service key
  password: 'nI2gdmXxZJxn', // replace with password from service key
//  path: { workspace_id: 'f20b4866-d714-40ad-8d35-ab65d670a93f' }, // replace with workspace ID
  version_date: '2018-01-09'
});

// Start conversation with empty message.
//  workspace_id: 'f20b4866-d714-40ad-8d35-ab65d670a93f',
//c4a752d4-f362-493c-a652-4922fb28ed9f
//conversation.message({workspace_id: 'f20b4866-d714-40ad-8d35-ab65d670a93f'}, processResponse);
conversation.message({workspace_id: 'c4a752d4-f362-493c-a652-4922fb28ed9f'}, processResponse);

// Process the conversation response.
function processResponse(err, response) {
  if (err) {
    console.error(err); // something went wrong
    return;
  }
var endConversation = false;

  // Check for action flags.
  if (response.output.action === 'display_time') {
    // User asked what time it is, so we output the local system time.
    console.log('The current time is ' + new Date().toLocaleTimeString());
  } else if (response.output.action === 'end_conversation') {
    // User said goodbye, so we're done.
    console.log(response.output.text[0]);
    endConversation = true;
  } else {
    // Display the output from dialog, if any.
    if (response.output.text.length != 0) {
        console.log(response.output.text[0]);
    }
  }

  // If we're not done, prompt for the next round of input.
  if (!endConversation) {
    var newMessageFromUser = prompt('>> ');
    conversation.message({
      input: { text: newMessageFromUser },
     workspace_id: 'c4a752d4-f362-493c-a652-4922fb28ed9f',
      // Send back the context to maintain state.
      context : response.context,
    }, processResponse)
  }
}
