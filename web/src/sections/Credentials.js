import React, { Component } from 'react';
import {Input, Password} from "../modules/Input.js"

export default class Credentials extends Component {
	constructor(props) {
		super(props);

		this.state = {display: true, r_username: "", r_fullname: "", r_password: "", s_username: "", s_password: "", show_register: false, processing: false};
	}

	componentWillMount() {
		// If session exists...
		// Login
		/*this.props.setUserInfo({
			username: this.state.r_username,
			password: this.state.r_password
		});

		this.hide();*/
	}

	handleSignUpUsername = (event) => {
		this.setState({r_username: event.target.value});
	}

	handleSignUpFullname = (event) => {
		this.setState({r_fullname: event.target.value});
	}

	handleSignUpPassword = (event) => {
		this.setState({r_password: event.target.value});
	}

	handleSignInUsername = (event) => {
		this.setState({s_username: event.target.value});
	}

	handleSignInPassword = (event) => {
		this.setState({s_password: event.target.value});
	}

	setSignState = (toggle) => {
		this.setState({show_register: toggle});
	}

	hide = () => {
		this.setState({processing: false, display: false});
	}

	handleSignIn = (e) => {
		e.preventDefault();
		this.setState({processing: true});

		setTimeout(() => {
			this.props.setUserInfo({
				username: this.state.s_username,
				name: this.state.s_username
			});
			
			this.hide();
		}, 700);
	}

	handleSignUp = (e) => {
		e.preventDefault();
		this.setState({processing: true});

		setTimeout(() => {
			this.props.setUserInfo({
				username: this.state.r_username,
				name: this.state.r_fullname
			});

			this.hide();
		}, 700);
	}
	
	logOut = () => {
		this.setState({r_fullname: "", r_username: "", r_password: "", s_username: "", s_password: "", display: true});
	}

	render() {
		return (
            <section className={`credentials${this.state.display ? "" : " hide"}`}>
				<div className="side"></div>
                <div className="main">
					<fieldset disabled={this.state.processing}>
						<div className={`credentials-container${this.state.show_register ? "" : " hide"}`}>
							<form onSubmit={this.handleSignUp}>
								<h1>Sign up with <span><span>D</span>EVE</span></h1>
								<div className="tagline">Have an account? <span onClick={() => this.setSignState(false)}>Sign In</span></div>
								<Input label="Full Name" placeholder="Full Name" value={this.state.r_fullname} onChange={this.handleSignUpFullname} type="text" />
								<Input label="Username" placeholder="Username" value={this.state.r_username} onChange={this.handleSignUpUsername} type="text" />
								<Password label="Password" placeholder="Password" value={this.state.r_password} onChange={this.handleSignUpPassword} type="password" />
								<input type="submit" value="Sign In" />
							</form>
						</div>
						<div className={`credentials-container${this.state.show_register ? " hide" : ""}`}>
							<form onSubmit={this.handleSignIn}>
								<h1>Sign in to <span><span>D</span>EVE</span></h1>
								<div className="tagline">Don't have an account? <span onClick={() => this.setSignState(true)}>Sign Up</span></div>
								<Input label="Username" placeholder="Username" value={this.state.s_username} onChange={this.handleSignInUsername} type="text" />
								<Password label="Password" placeholder="Password" value={this.state.s_password} onChange={this.handleSignInPassword} type="password" />
								<input type="submit" value="Sign In" />
							</form>
						</div>
					</fieldset>
				</div>
            </section>
		);
	}
}