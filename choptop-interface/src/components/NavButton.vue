<template>
	<div :class="getClass()" v-on:click="doAction()">
		{{direction}} button:
		{{pressed}}
	</div>
</template>

<script>
	export default {
	  name: 'nav-button',
	  props: ['pressed', 'direction', 'eventBus'],
	  watch: { 
	  	pressed : function(newVal, oldVal){
	  		console.log('Pressed for ' + this.direction + ' changed: ', newVal, ' | was: ', oldVal)
	  	}
	  },

	  data () {
	    return {
	    	pressStart: 0,
	      
	    }
	  },

	  methods: {
	  	getClass : function(){
	  		return "nav-button " + "nav-button-" + this.direction + " pressed-" + this.pressed
	  	},
	  	doAction(){
	  		this.eventBus.$emit("pressed",this.direction)
	  	}
	  },


	}
</script>



<style scoped>

	@keyframes buttonPress {
    0%   {background-color: green;}
    70%  {background-color: orange;}
    95% {background-color: red;}
    100%{background-color: green;}
}

	.nav-button{
		border:3px solid #004a63;
		color:white;
	}

	.nav-button-up{
		height:20px;
	}

	.nav-button-left{
		display:inline-block;
		height:440px;
		width:20px;
		float:left;
	}

	.nav-button-right{
		display:inline-block;
		height:440px;
		width:20px;
		float:left;
	}

	.nav-button-down{
		clear:both;
		height:20px;
	}

	.pressed-true{
		animation-name: buttonPress;
    	animation-duration: 2s;
    	background-color: green;
	}

	.pressed-false{
		background:green;
		background:white;
	}
</style>
