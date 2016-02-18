if (Meteor.isServer) {

States.before.insert(function(userId, doc){

	doc.createdAt = new Date();

});

/*
Events.before.insert(function(){

	updated_at =new Date();

});
*/
}
