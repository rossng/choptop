<template>
	<div :class="getClass()">
		<h1>{{recipe.title}}</h1>
		<h2>Difficulty: {{recipe.difficulty}}</h2>
		<h2>Time: {{recipe.time}} minutes</h2>
	</div>
</template>

<script>
	export default {
	  name: 'recipe',
	  props:['recipe', 'selected'],
	  data () {
	    return {
	      
	    }
	  },

	  methods: {
	  	getClass : function(){
	  		var divClass = "recipeOverview"
	  		if(this.selected){
	  			divClass += " selected"
	  		}
	  		return divClass
	  	}
	  }
	}
</script>



<style scoped>
	h1{
	  
	}

	.recipeOverview{
		border:1px black solid;
		margin:10px;
	}

	.recipeOverview.selected{
		background:lightblue;
	}
	
</style>
