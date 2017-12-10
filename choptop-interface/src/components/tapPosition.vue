<template>
	<div class="pos">
		Finger coordonates:

	</div>
</template>

<script>
	// import AnimatedInt from './AnimatedInteger'
	export default {
	  name: 'tapPosition',
	  props:['eventBus'],
	  components:{
	  	// AnimatedInt
	  },

	  created (){
	  	this.eventBus.$on("pos", this.handleNewPos);
	  },
	  methods:{
	  	handleNewPos(weight){
	  		console.log("x - y position: " + posX +" - "+posY)
	  		this.currentWeight = weight;
	  	},
	  },
	  data () {
	    return {
			posX:0.25,
			posY:0.25,
	    }
	  }
	  
	}
</script>



<style scoped>
	.tapPosition{
		position: fixed;
		left: 10px;
		top:10px;
	}
	
</style>
