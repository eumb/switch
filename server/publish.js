Meteor.publish('allevents',function(){
	return Events.find();
});

Meteor.publish('allstates',function(){
	return States.find();
});
