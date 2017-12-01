<template>
	<div :class="getClass()">
		<div v-if="hovered && !selected">
			<h1>{{recipe.title}}</h1>
			<h2>Difficulty: {{recipe.difficulty}}</h2>
			<h2>Time: {{recipe.time}} minutes</h2>
		</div>
		<div v-if="selected">
			<h1>{{recipe.title}}</h1>

			<div id="ingredients">
				Ingredients
				<div v-for="(ingredient, idx) in this.recipe.ingredients" class="ingredients">
					{{getIngredientText(ingredient)}}
				</div>
			</div>


			<div id="steps">
				Step {{this.selectedStep +1}}/{{this.recipe.steps.length -1}}
				<div v-for="(step, idx) in this.recipe.steps" class="step">
					<div v-if="idx == selectedStep">
						{{step.text}}
					</div>
				</div>
			</div>

		</div>
	</div>
</template>

<script>
	export default {
	  name: 'recipe',
	  props:['recipe', 'hovered', 'selected'],
	  data () {
	    return {
	      selectedStep:0,
	    }
	  },

	  methods: {
	  	nextStep: function(){
	  		if(selectedStep < length(this.recipe.steps)){
	  			selectedStep++;
	  		}
	  	},
	  	prevStep: function(){
	  		if(selectedStep > 0){
	  			selectedStep--;
	  		}
	  	},
	  	getClass : function(){
	  		var divClass = "recipeOverview"
	  		if(this.hovered && !this.selected){
	  			divClass += " hovered"
	  		}

	  		if(this.selected){
	  			divClass += " selected"
	  		}
	  		return divClass
	  	},

	  	getIngredientText(ingredient){
	  		//this code isn't nice, sorry
	  		var text = "";
	  		if(ingredient.units.length == 2){
	  			if(ingredient.units[0] == "each"){
	  				var quantity = ingredient.quantity[0]
	  				text += quantity + " "
	  				text += ingredient.name
	  				if(quantity > 1 && ingredient.name[ingredient.name.length -1] != s){
	  					text += "s" //pluralise names
	  				}
	  			}else{

	  			}
	  		}
	  		return text
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

	.recipeOverview.hovered{
		background:lightblue;
	}

	.recipeOverview.selected{
		background: green;
	}
	
</style>
