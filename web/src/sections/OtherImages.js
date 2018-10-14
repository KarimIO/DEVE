import React, { Component } from 'react';

export default class MyImages extends Component {
	render() {
		return (
			<section className="main-section">
				<div className="gallery">
					{this.props.gallery.map((e, i) => {
						return <div key={i} style={{backgroundImage: "url(/img/"+e.url+")"}}>
							<div>
								<span>
									{e.views > 0 && <i>{e.views} views left</i>}
									{e.views === 0 && <button>Request</button>}
									{e.views > 0 && <button>Request More</button>}
								</span>
							</div>
						</div>;
					})}
				</div>
			</section>
		);
	}
}