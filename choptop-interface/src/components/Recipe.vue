<template>
	<div :class="getClass()">
		<div class="title">
			<h1>{{recipe.title}}</h1>
		</div>

		<div class="ingredients">
			Ingredients
		</div>

		<div class="method">
			Method
		</div>
		<h2>Difficulty: {{recipe.difficulty}}</h2>
		<h3>Time: {{recipe.time}} minutes</h3>
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
	  		var divClass = "recipe"
	  		return divClass
	  	}
	  }
	}
</script>



<style scoped>
	h1{

	}

	.recipe{
		border:1px #004a63 solid;
		margin:10px;
	}

	.recipeOverview.opened{
		background:#68b4b2;

	}

</style>
